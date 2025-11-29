#include <engine/render/model_cache.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <unordered_map>

namespace engine {

namespace {

MeshData process_mesh(aiMesh* mesh) {
    MeshData data;

    // Reserve space
    data.positions.reserve(mesh->mNumVertices);
    if (mesh->HasNormals()) data.normals.reserve(mesh->mNumVertices);
    if (mesh->HasTextureCoords(0)) data.uvs.reserve(mesh->mNumVertices);
    if (mesh->HasVertexColors(0)) data.colors.reserve(mesh->mNumVertices);

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
std::shared_ptr<Texture> load_embedded_texture(const aiTexture* ai_tex) {
    if (ai_tex->mHeight == 0) {
        // Compressed texture (PNG, JPG, etc.) stored as raw bytes
        return std::make_shared<Texture>(
            reinterpret_cast<const unsigned char*>(ai_tex->pcData),
            ai_tex->mWidth  // mWidth contains the byte size for compressed textures
        );
    } else {
        // Uncompressed RGBA texture - need to upload raw pixel data
        // This is less common in GLB files
        GLuint tex_id;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     ai_tex->mWidth, ai_tex->mHeight, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, ai_tex->pcData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Note: This creates a texture with just the ID, we'd need a different Texture constructor
        // For now, use the memory-based constructor with raw data
        // This path is rarely used for GLB files
        std::cerr << "Warning: Uncompressed embedded texture not fully supported" << std::endl;
        return nullptr;
    }
}

struct ProcessedMesh {
    std::shared_ptr<Mesh> mesh;
    unsigned int material_index;
};

void process_node(aiNode* node, const aiScene* scene, std::vector<ProcessedMesh>& meshes) {
    // Process all meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        MeshData data = process_mesh(ai_mesh);
        ProcessedMesh pm;
        pm.mesh = std::make_shared<Mesh>(data);
        pm.material_index = ai_mesh->mMaterialIndex;
        meshes.push_back(pm);
    }

    // Process children
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        process_node(node->mChildren[i], scene, meshes);
    }
}

}  // namespace

std::shared_ptr<Model> ModelCache::load(const std::string& filepath) {
    auto it = _cache.find(filepath);
    if (it != _cache.end()) {
        return it->second;
    }

    auto model = load_from_file(filepath);
    _cache[filepath] = model;
    return model;
}

void ModelCache::clear() {
    _cache.clear();
}

bool ModelCache::contains(const std::string& filepath) const {
    return _cache.find(filepath) != _cache.end();
}

std::shared_ptr<Model> ModelCache::load_from_file(const std::string& filepath) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error("Failed to load model: " + filepath + " - " + importer.GetErrorString());
    }

    auto model = std::make_shared<Model>();

    std::cout << "Loading model: " << filepath << std::endl;
    std::cout << "  Meshes: " << scene->mNumMeshes << std::endl;
    std::cout << "  Materials: " << scene->mNumMaterials << std::endl;
    std::cout << "  Textures: " << scene->mNumTextures << std::endl;

    // Load embedded textures first (indexed by "*N" in material texture paths)
    std::unordered_map<unsigned int, std::shared_ptr<Texture>> embedded_textures;
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        std::cout << "  Embedded texture " << i << ": "
                  << scene->mTextures[i]->mWidth << "x" << scene->mTextures[i]->mHeight
                  << " format: " << scene->mTextures[i]->achFormatHint << std::endl;
        auto tex = load_embedded_texture(scene->mTextures[i]);
        if (tex) {
            embedded_textures[i] = tex;
            model->_textures.push_back(tex);  // Keep ownership
        }
    }

    // Process meshes with material info
    std::vector<ProcessedMesh> processed_meshes;
    process_node(scene->mRootNode, scene, processed_meshes);

    // Build material -> texture mapping
    // For each material, find its diffuse texture
    std::unordered_map<unsigned int, GLuint> material_to_texture;
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* mat = scene->mMaterials[i];
        aiString mat_name;
        mat->Get(AI_MATKEY_NAME, mat_name);
        std::cout << "  Material " << i << ": " << mat_name.C_Str() << std::endl;
        std::cout << "    DIFFUSE textures: " << mat->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
        std::cout << "    BASE_COLOR textures: " << mat->GetTextureCount(aiTextureType_BASE_COLOR) << std::endl;

        // Check for diffuse/base color texture
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString tex_path;
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path);
            std::cout << "    DIFFUSE path: " << tex_path.C_Str() << std::endl;

            std::string path_str = tex_path.C_Str();
            // Embedded textures in GLB are referenced as "*0", "*1", etc.
            if (!path_str.empty() && path_str[0] == '*') {
                unsigned int tex_index = std::stoul(path_str.substr(1));
                auto it = embedded_textures.find(tex_index);
                if (it != embedded_textures.end()) {
                    material_to_texture[i] = it->second->id();
                    std::cout << "    -> mapped to texture " << tex_index << std::endl;
                }
            }
        }
        // Also check BASE_COLOR for PBR materials (glTF)
        if (mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
            aiString tex_path;
            mat->GetTexture(aiTextureType_BASE_COLOR, 0, &tex_path);
            std::cout << "    BASE_COLOR path: " << tex_path.C_Str() << std::endl;

            std::string path_str = tex_path.C_Str();
            if (!path_str.empty() && path_str[0] == '*') {
                unsigned int tex_index = std::stoul(path_str.substr(1));
                auto it = embedded_textures.find(tex_index);
                if (it != embedded_textures.end()) {
                    material_to_texture[i] = it->second->id();
                    std::cout << "    -> mapped to texture " << tex_index << std::endl;
                }
            }
        }
    }

    // Populate model meshes and materials
    for (const auto& pm : processed_meshes) {
        model->_meshes.push_back(pm.mesh);

        // Create StandardMaterial for this mesh
        auto material = std::make_shared<StandardMaterial>(
            glm::vec3(1.0f),  // white albedo (texture will provide color)
            glm::vec3(1.0f),  // specular
            0.0f,             // metallic
            0.5f              // roughness
        );

        // Set texture if available
        auto tex_it = material_to_texture.find(pm.material_index);
        if (tex_it != material_to_texture.end()) {
            material->albedo_map = tex_it->second;
        }

        model->_materials.push_back(material);
    }

    // TODO: Process animations and skeleton

    return model;
}

}  // namespace engine
