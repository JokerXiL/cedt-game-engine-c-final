#include <engine/render/standard_material.hpp>

#include <iostream>

namespace engine {

StandardMaterial::StandardMaterial(
    const glm::vec3& albedo,
    const glm::vec3& specular,
    float metallic,
    float roughness
)
    : albedo(albedo),
      specular_color(specular),
      metallic(metallic),
      roughness(roughness),
      albedo_map(0),
      specular_map(0),
      metallic_roughness_map(0),
      normal_map(0),
      light_pos(10.0f, 10.0f, 10.0f),
      light_color(1.0f, 1.0f, 1.0f),
      view_pos(0.0f, 0.0f, 0.0f),
      projection(1.0f),
      view(1.0f) {

    // Create single shader program for both static and skinned rendering
    _shader_program_id = create_shader_program("engine/shaders/pbr.vert", "engine/shaders/pbr.frag");

    if (_shader_program_id == 0) {
        std::cout << "ERROR::STANDARD_MATERIAL::Failed to create shader program" << std::endl;
    }
}

void StandardMaterial::set_common_uniforms(const glm::mat4& model) {
    use_program(_shader_program_id);

    // Transformation matrices
    set_mat4(_shader_program_id, "model", model);
    set_mat4(_shader_program_id, "view", view);
    set_mat4(_shader_program_id, "projection", projection);

    // Material properties
    set_vec3(_shader_program_id, "albedo", albedo);
    set_vec3(_shader_program_id, "specularColor", specular_color);
    set_float(_shader_program_id, "metallic", metallic);
    set_float(_shader_program_id, "roughness", roughness);

    // Lighting
    set_vec3(_shader_program_id, "lightPos", light_pos);
    set_vec3(_shader_program_id, "lightColor", light_color);
    set_vec3(_shader_program_id, "viewPos", view_pos);

    // Texture flags
    set_bool(_shader_program_id, "useAlbedoMap", albedo_map != 0);
    set_bool(_shader_program_id, "useSpecularMap", specular_map != 0);
    set_bool(_shader_program_id, "useMetallicMap", metallic_roughness_map != 0);
    set_bool(_shader_program_id, "useRoughnessMap", metallic_roughness_map != 0);
    set_bool(_shader_program_id, "useNormalMap", normal_map != 0);

    // Bind textures
    bind_textures();
}

void StandardMaterial::set_bone_transforms(const Skeleton* skeleton) {
    if (skeleton && skeleton->get_bone_count() > 0) {
        // Upload bone transforms (max 100 bones)
        auto final_transforms = skeleton->get_final_transforms();
        size_t bone_count = std::min(final_transforms.size(), size_t(100));

        for (size_t i = 0; i < bone_count; ++i) {
            std::string uniform_name = "boneTransforms[" + std::to_string(i) + "]";
            set_mat4(_shader_program_id, uniform_name, final_transforms[i]);
        }
    }
    // Note: If skeleton is null or has no bones, identity matrices are already set in constructor
}

void StandardMaterial::bind_textures() {
    if (albedo_map != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo_map);
        set_int(_shader_program_id, "albedoMap", 0);
    }

    if (specular_map != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);
        set_int(_shader_program_id, "specularMap", 1);
    }

    if (metallic_roughness_map != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallic_roughness_map);
        set_int(_shader_program_id, "metallicRoughnessMap", 2);
    }

    if (normal_map != 0) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normal_map);
        set_int(_shader_program_id, "normalMap", 3);
    }
}

void StandardMaterial::render(const Mesh& mesh, const glm::mat4& transform) {
    if (_shader_program_id == 0) {
        std::cout << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    // Set all uniforms
    set_common_uniforms(transform);

    // Disable skinning for static meshes
    set_bool(_shader_program_id, "useSkinning", false);

    // Draw the mesh
    mesh.draw();
}

void StandardMaterial::render_skinned(const Mesh& mesh, const glm::mat4& transform, const Skeleton& skeleton) {
    if (_shader_program_id == 0) {
        std::cout << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    // Set common uniforms
    set_common_uniforms(transform);

    // Enable skinning and upload bone transforms
    set_bool(_shader_program_id, "useSkinning", true);
    set_bone_transforms(&skeleton);

    // Draw the mesh
    mesh.draw();
}

}  // namespace engine
