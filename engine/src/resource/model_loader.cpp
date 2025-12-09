#include <engine/resource/loaders/model_loader.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/texture.hpp>
#include <engine/pbr/skeleton.hpp>
#include <engine/pbr/animation.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <iostream>
#include <unordered_map>

namespace engine::resource {

namespace {

pbr::MeshData process_mesh(aiMesh* mesh) {
    pbr::MeshData data;

    // Reserve space
    data.positions.reserve(mesh->mNumVertices);
    if (mesh->HasNormals()) data.normals.reserve(mesh->mNumVertices);
    if (mesh->HasTextureCoords(0)) data.uvs.reserve(mesh->mNumVertices);
    if (mesh->HasVertexColors(0)) data.colors.reserve(mesh->mNumVertices);

    // Initialize bone weights and indices with defaults
    // If mesh has no bones, set weight to 1.0 so vertices render correctly
    // when accidentally used with skinning enabled
    bool has_bones = mesh->mNumBones > 0;
    glm::vec4 default_weight = has_bones ? glm::vec4(0.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    data.joint_weights.resize(mesh->mNumVertices, default_weight);
    data.joint_indices.resize(mesh->mNumVertices, glm::ivec4(0));

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        data.positions.emplace_back(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals()) {
            data.normals.emplace_back(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        if (mesh->HasTextureCoords(0)) {
            // Flip V coordinate for glTF (OpenGL expects bottom-left origin)
            data.uvs.emplace_back(
                mesh->mTextureCoords[0][i].x,
                1.0f - mesh->mTextureCoords[0][i].y
            );
        }

        if (mesh->HasVertexColors(0)) {
            data.colors.emplace_back(
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b,
                mesh->mColors[0][i].a
            );
        } else {
            // Default white color
            data.colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    // Process bone weights and indices
    for (unsigned int bone_idx = 0; bone_idx < mesh->mNumBones; ++bone_idx) {
        aiBone* bone = mesh->mBones[bone_idx];

        for (unsigned int weight_idx = 0; weight_idx < bone->mNumWeights; ++weight_idx) {
            aiVertexWeight& vw = bone->mWeights[weight_idx];
            unsigned int vertex_id = vw.mVertexId;
            if (vertex_id < mesh->mNumVertices) {
                // Find the first available slot for this bone weight
                for (int slot = 0; slot < 4; ++slot) {
                    if (data.joint_weights[vertex_id][slot] == 0.0f) {
                        data.joint_weights[vertex_id][slot] = vw.mWeight;
                        data.joint_indices[vertex_id][slot] = bone_idx;  // Local bone index
                        break;
                    }
                }
            }
        }
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            data.indices.push_back(face.mIndices[j]);
        }
    }

    return data;
}

/// Load embedded texture from Assimp scene
std::shared_ptr<pbr::Texture> load_embedded_texture(const aiTexture* ai_tex) {
    if (ai_tex->mHeight == 0) {
        // Compressed texture (PNG, JPG, etc.) stored as raw bytes
        return std::make_shared<pbr::Texture>(
            reinterpret_cast<const unsigned char*>(ai_tex->pcData),
            ai_tex->mWidth  // mWidth contains the byte size for compressed textures
        );
    } else {
        // Uncompressed RGBA texture - less common in GLB files
        std::cerr << "Warning: Uncompressed embedded texture not fully supported" << std::endl;
        return nullptr;
    }
}

struct ProcessedMesh {
    std::shared_ptr<pbr::Mesh> mesh;
    unsigned int material_index;
};

/// Convert aiMatrix4x4 to glm::mat4
glm::mat4 aimat_to_glm(const aiMatrix4x4& ai_mat) {
    glm::mat4 result;
    result[0][0] = ai_mat.a1; result[1][0] = ai_mat.a2; result[2][0] = ai_mat.a3; result[3][0] = ai_mat.a4;
    result[0][1] = ai_mat.b1; result[1][1] = ai_mat.b2; result[2][1] = ai_mat.b3; result[3][1] = ai_mat.b4;
    result[0][2] = ai_mat.c1; result[1][2] = ai_mat.c2; result[2][2] = ai_mat.c3; result[3][2] = ai_mat.c4;
    result[0][3] = ai_mat.d1; result[1][3] = ai_mat.d2; result[2][3] = ai_mat.d3; result[3][3] = ai_mat.d4;
    return result;
}

/// Build complete node hierarchy (including non-bone nodes)
pbr::SkeletonNode build_node_hierarchy(aiNode* ai_node) {
    pbr::SkeletonNode node;
    node.name = ai_node->mName.C_Str();
    node.transformation = aimat_to_glm(ai_node->mTransformation);

    node.children.reserve(ai_node->mNumChildren);
    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        node.children.push_back(build_node_hierarchy(ai_node->mChildren[i]));
    }

    return node;
}

/// Extract skeleton (bones and bind pose) from the scene
std::shared_ptr<pbr::Skeleton> extract_skeleton(const aiScene* scene) {
    if (!scene || scene->mNumMeshes == 0) {
        return nullptr;
    }

    auto bone_index_map = std::make_shared<std::unordered_map<std::string, int>>();
    auto bind_pose = std::make_shared<std::vector<glm::mat4>>();
    int bone_count = 0;

    // Extract all unique bones from all meshes
    for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx) {
        aiMesh* ai_mesh = scene->mMeshes[mesh_idx];
        for (unsigned int bone_idx = 0; bone_idx < ai_mesh->mNumBones; ++bone_idx) {
            aiBone* bone = ai_mesh->mBones[bone_idx];
            std::string bone_name = bone->mName.C_Str();

            if (bone_index_map->find(bone_name) == bone_index_map->end()) {
                (*bone_index_map)[bone_name] = bone_count;
                glm::mat4 offset_matrix = aimat_to_glm(bone->mOffsetMatrix);
                bind_pose->push_back(offset_matrix);
                bone_count++;
            }
        }
    }

    if (bone_count == 0) {
        return nullptr;
    }

    auto skeleton = std::make_shared<pbr::Skeleton>(bone_count, bind_pose);
    skeleton->bone_index_map = bone_index_map;

    if (scene->mRootNode) {
        skeleton->root_node = std::make_shared<pbr::SkeletonNode>(
            build_node_hierarchy(scene->mRootNode)
        );
    }

    return skeleton;
}

/// Extract animations from the scene
std::vector<std::shared_ptr<pbr::AnimationClip>> extract_animations(const aiScene* scene) {
    std::vector<std::shared_ptr<pbr::AnimationClip>> animations;

    if (!scene || scene->mNumAnimations == 0) {
        return animations;
    }

    for (unsigned int anim_idx = 0; anim_idx < scene->mNumAnimations; ++anim_idx) {
        aiAnimation* ai_anim = scene->mAnimations[anim_idx];

        auto clip = std::make_shared<pbr::AnimationClip>();
        clip->set_name(ai_anim->mName.C_Str());
        clip->set_duration(static_cast<float>(ai_anim->mDuration));
        clip->set_ticks_per_second(ai_anim->mTicksPerSecond > 0.0
            ? static_cast<float>(ai_anim->mTicksPerSecond)
            : 25.0f);

        // Process each animation channel
        for (unsigned int channel_idx = 0; channel_idx < ai_anim->mNumChannels; ++channel_idx) {
            aiNodeAnim* ai_channel = ai_anim->mChannels[channel_idx];

            pbr::AnimationChannel channel;
            channel.bone_name = ai_channel->mNodeName.C_Str();

            // Extract position keyframes
            channel.position_keys.reserve(ai_channel->mNumPositionKeys);
            for (unsigned int key_idx = 0; key_idx < ai_channel->mNumPositionKeys; ++key_idx) {
                const aiVectorKey& ai_key = ai_channel->mPositionKeys[key_idx];
                pbr::PositionKey key;
                key.time = static_cast<float>(ai_key.mTime);
                key.value = glm::vec3(ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
                channel.position_keys.push_back(key);
            }

            // Extract rotation keyframes
            channel.rotation_keys.reserve(ai_channel->mNumRotationKeys);
            for (unsigned int key_idx = 0; key_idx < ai_channel->mNumRotationKeys; ++key_idx) {
                const aiQuatKey& ai_key = ai_channel->mRotationKeys[key_idx];
                pbr::RotationKey key;
                key.time = static_cast<float>(ai_key.mTime);
                key.value = glm::quat(ai_key.mValue.w, ai_key.mValue.x,
                                     ai_key.mValue.y, ai_key.mValue.z);
                channel.rotation_keys.push_back(key);
            }

            // Extract scale keyframes
            channel.scale_keys.reserve(ai_channel->mNumScalingKeys);
            for (unsigned int key_idx = 0; key_idx < ai_channel->mNumScalingKeys; ++key_idx) {
                const aiVectorKey& ai_key = ai_channel->mScalingKeys[key_idx];
                pbr::ScaleKey key;
                key.time = static_cast<float>(ai_key.mTime);
                key.value = glm::vec3(ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
                channel.scale_keys.push_back(key);
            }

            clip->add_channel(std::move(channel));
        }

        animations.push_back(clip);
    }

    return animations;
}

}  // namespace

ModelLoader::ResourcePtr ModelLoader::operator()(const std::string& filepath) const {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_GenBoundingBoxes
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Failed to load model: " << filepath << " - " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    auto model = std::make_shared<pbr::Model>();

    // Load embedded textures (indexed by "*N" in material texture paths)
    std::unordered_map<unsigned int, std::shared_ptr<pbr::Texture>> embedded_textures;
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        auto tex = load_embedded_texture(scene->mTextures[i]);
        if (tex) {
            embedded_textures[i] = tex;
            model->_textures.push_back(tex);
        }
    }

    // Extract skeleton
    model->_skeleton = extract_skeleton(scene);

    // Process meshes
    std::vector<std::pair<pbr::MeshData, unsigned int>> mesh_data_list;

    std::function<void(aiNode*)> process_meshes_only = [&](aiNode* node) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
            pbr::MeshData data = process_mesh(ai_mesh);
            mesh_data_list.emplace_back(std::move(data), ai_mesh->mMaterialIndex);
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            process_meshes_only(node->mChildren[i]);
        }
    };
    process_meshes_only(scene->mRootNode);

    // Remap bone indices if we have a skeleton
    if (model->_skeleton) {
        for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx) {
            if (mesh_idx >= mesh_data_list.size()) break;

            aiMesh* ai_mesh = scene->mMeshes[mesh_idx];
            auto& mesh_data = mesh_data_list[mesh_idx].first;

            // Create local-to-global mapping for this mesh's bones
            std::unordered_map<int, int> local_to_global;
            for (unsigned int local_idx = 0; local_idx < ai_mesh->mNumBones; ++local_idx) {
                std::string bone_name = ai_mesh->mBones[local_idx]->mName.C_Str();
                int global_idx = model->_skeleton->get_bone_index(bone_name);
                if (global_idx >= 0) {
                    local_to_global[local_idx] = global_idx;
                }
            }

            // Remap joint indices
            for (auto& joint_idx : mesh_data.joint_indices) {
                for (int slot = 0; slot < 4; ++slot) {
                    int local_idx = joint_idx[slot];
                    auto it = local_to_global.find(local_idx);
                    if (it != local_to_global.end()) {
                        joint_idx[slot] = it->second;
                    }
                }
            }
        }
    }

    // Create GPU meshes
    std::vector<ProcessedMesh> processed_meshes;
    for (const auto& [data, mat_idx] : mesh_data_list) {
        ProcessedMesh pm;
        pm.mesh = std::make_shared<pbr::Mesh>(data);
        pm.material_index = mat_idx;
        processed_meshes.push_back(pm);
    }

    // Build material -> texture mapping
    std::unordered_map<unsigned int, GLuint> material_to_texture;
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* mat = scene->mMaterials[i];

        // Check for diffuse/base color texture
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString tex_path;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path);
            std::string path_str = tex_path.C_Str();
            if (!path_str.empty() && path_str[0] == '*') {
                unsigned int tex_index = std::stoul(path_str.substr(1));
                auto it = embedded_textures.find(tex_index);
                if (it != embedded_textures.end()) {
                    material_to_texture[i] = it->second->id();
                }
            }
        }
        // Also check BASE_COLOR for PBR materials (glTF)
        if (mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
            aiString tex_path;
            mat->GetTexture(aiTextureType_BASE_COLOR, 0, &tex_path);
            std::string path_str = tex_path.C_Str();
            if (!path_str.empty() && path_str[0] == '*') {
                unsigned int tex_index = std::stoul(path_str.substr(1));
                auto it = embedded_textures.find(tex_index);
                if (it != embedded_textures.end()) {
                    material_to_texture[i] = it->second->id();
                }
            }
        }
    }

    // Populate model meshes and materials
    for (const auto& pm : processed_meshes) {
        model->_meshes.push_back(pm.mesh);

        auto material = std::make_shared<pbr::StandardMaterial>(
            _shader_loader,
            glm::vec3(1.0f),  // white albedo
            glm::vec3(1.0f),  // specular
            0.0f,             // metallic
            0.5f              // roughness
        );

        auto tex_it = material_to_texture.find(pm.material_index);
        if (tex_it != material_to_texture.end()) {
            material->albedo_map = tex_it->second;
        }

        model->_materials.push_back(material);
    }

    // Extract animations
    model->_animations = extract_animations(scene);

    return model;
}

}  // namespace engine::resource
