#include <engine/pbr/standard_material.hpp>
#include <engine/pbr/scene.hpp>

#include <iostream>
#include <cmath>

namespace engine::pbr {

StandardMaterial::StandardMaterial(
    ShaderLoadFunc shader_loader,
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
      normal_map(0) {

    // Load shaders using the provided loader function (shared across all StandardMaterial instances)
    _shader = shader_loader("engine/shaders/pbr.vert", "engine/shaders/pbr.frag");
    _shadow_shader = shader_loader("engine/shaders/shadow.vert", "engine/shaders/shadow.frag");

    if (!_shader || !_shader->valid()) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Failed to load PBR shader" << std::endl;
    }
    if (!_shadow_shader || !_shadow_shader->valid()) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Failed to load shadow shader" << std::endl;
    }
}

// ============================================================================
// Scene-based rendering
// ============================================================================

void StandardMaterial::set_light_uniforms(const Scene& scene) {
    const auto& lights = scene.lights();
    int num_lights = static_cast<int>(std::min(lights.size(), size_t(MAX_LIGHTS)));

    _shader->set_int("numLights", num_lights);
    _shader->set_vec3("ambientColor", scene.ambient());

    for (int i = 0; i < num_lights; ++i) {
        const Light& light = lights[static_cast<size_t>(i)];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        _shader->set_int(prefix + "type", static_cast<int>(light.type));
        _shader->set_vec3(prefix + "position", light.position);
        _shader->set_vec3(prefix + "direction", light.direction);
        _shader->set_vec3(prefix + "color", light.color);
        _shader->set_float(prefix + "intensity", light.intensity);
        _shader->set_float(prefix + "constant", light.attenuation_constant);
        _shader->set_float(prefix + "linear", light.attenuation_linear);
        _shader->set_float(prefix + "quadratic", light.attenuation_quadratic);
        // Store cos(angle) for spot lights - shader expects cosine values
        _shader->set_float(prefix + "innerCutoff", std::cos(light.inner_cutoff));
        _shader->set_float(prefix + "outerCutoff", std::cos(light.outer_cutoff));
        _shader->set_int(prefix + "shadowMapIndex", light.shadow_map_index);
    }
}

void StandardMaterial::set_shadow_uniforms(const Scene& scene) {
    const ShadowData* shadow_data = scene.shadow_data();
    if (!shadow_data) return;

    // Count shadow maps by type
    int num_2d_shadows = 0;
    int num_cube_shadows = 0;

    for (const auto& light : scene.lights()) {
        if (light.casts_shadow && light.shadow_map_index >= 0) {
            if (light.type == LightType::Point) {
                if (light.shadow_map_index < MAX_SHADOW_CUBEMAPS) {
                    num_cube_shadows = std::max(num_cube_shadows, light.shadow_map_index + 1);
                }
            } else {
                if (light.shadow_map_index < MAX_SHADOW_MAPS) {
                    num_2d_shadows = std::max(num_2d_shadows, light.shadow_map_index + 1);
                }
            }
        }
    }

    _shader->set_int("numShadowMaps", num_2d_shadows);
    _shader->set_int("numShadowCubeMaps", num_cube_shadows);

    // Set light space matrices for 2D shadow maps
    for (int i = 0; i < num_2d_shadows; ++i) {
        std::string name = "lightSpaceMatrices[" + std::to_string(i) + "]";
        _shader->set_mat4(name, shadow_data->light_space_matrices[static_cast<size_t>(i)]);
    }

    // Set point light far planes for cubemap shadows
    for (int i = 0; i < num_cube_shadows; ++i) {
        std::string name = "pointLightFarPlanes[" + std::to_string(i) + "]";
        _shader->set_float(name, shadow_data->point_light_far_planes[static_cast<size_t>(i)]);
    }

    // Bind shadow texture samplers
    int shadow_unit = shadow_data->shadow_map_texture_unit;
    for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
        std::string name = "shadowMaps[" + std::to_string(i) + "]";
        _shader->set_int(name, shadow_unit + i);
    }

    int cubemap_unit = shadow_data->shadow_cubemap_texture_unit;
    for (int i = 0; i < MAX_SHADOW_CUBEMAPS; ++i) {
        std::string name = "shadowCubeMaps[" + std::to_string(i) + "]";
        _shader->set_int(name, cubemap_unit + i);
    }
}

void StandardMaterial::set_common_uniforms(const glm::mat4& model, const Scene& scene) {
    _shader->use();

    const Camera* camera = scene.camera();
    if (!camera) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Scene has no camera" << std::endl;
        return;
    }

    // Transformation matrices
    _shader->set_mat4("model", model);
    _shader->set_mat4("view", camera->view());
    _shader->set_mat4("projection", camera->projection());

    // View position
    _shader->set_vec3("viewPos", camera->position());

    // Material properties
    _shader->set_vec3("albedo", albedo);
    _shader->set_vec3("specularColor", specular_color);
    _shader->set_float("metallic", metallic);
    _shader->set_float("roughness", roughness);

    // Texture flags
    _shader->set_bool("useAlbedoMap", albedo_map != 0);
    _shader->set_bool("useSpecularMap", specular_map != 0);
    _shader->set_bool("useMetallicMap", metallic_roughness_map != 0);
    _shader->set_bool("useRoughnessMap", metallic_roughness_map != 0);
    _shader->set_bool("useNormalMap", normal_map != 0);

    // Lighting
    set_light_uniforms(scene);

    // Shadow maps
    set_shadow_uniforms(scene);

    // Bind material textures
    bind_textures();
}

void StandardMaterial::render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) {
    if (!_shader || !_shader->valid()) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms(transform, scene);
    _shader->set_bool("useSkinning", false);
    draw_mesh(mesh);
}

void StandardMaterial::render_skinned(const Mesh& mesh, const glm::mat4& transform,
                                       const Skeleton& skeleton, const Scene& scene) {
    if (!_shader || !_shader->valid()) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms(transform, scene);
    _shader->set_bool("useSkinning", true);
    set_bone_transforms(&skeleton, *_shader);
    draw_mesh(mesh);
}

// ============================================================================
// Shadow rendering
// ============================================================================

bool StandardMaterial::begin_shadow_pass(const glm::mat4& transform,
                                          const glm::mat4& light_space_matrix,
                                          const Skeleton* skeleton,
                                          bool is_point_light,
                                          const glm::vec3& light_pos,
                                          float far_plane) {
    if (!_shadow_shader || !_shadow_shader->valid()) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Shadow shader not initialized" << std::endl;
        return false;
    }

    _shadow_shader->use();
    _shadow_shader->set_mat4("model", transform);
    _shadow_shader->set_mat4("lightSpaceMatrix", light_space_matrix);
    _shadow_shader->set_bool("useSkinning", skeleton != nullptr);
    _shadow_shader->set_bool("isPointLight", is_point_light);

    if (is_point_light) {
        _shadow_shader->set_vec3("lightPos", light_pos);
        _shadow_shader->set_float("farPlane", far_plane);
    }

    if (skeleton) {
        set_bone_transforms(skeleton, *_shadow_shader);
    }

    return true;
}

void StandardMaterial::render_shadow(const Mesh& mesh, const glm::mat4& transform,
                                      const glm::mat4& light_space_matrix) {
    if (begin_shadow_pass(transform, light_space_matrix, nullptr, false)) {
        draw_mesh(mesh);
    }
}

void StandardMaterial::render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                                              const Skeleton& skeleton,
                                              const glm::mat4& light_space_matrix) {
    if (begin_shadow_pass(transform, light_space_matrix, &skeleton, false)) {
        draw_mesh(mesh);
    }
}

void StandardMaterial::render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                                           const glm::mat4& light_space_matrix,
                                           const glm::vec3& light_pos, float far_plane) {
    if (begin_shadow_pass(transform, light_space_matrix, nullptr, true, light_pos, far_plane)) {
        draw_mesh(mesh);
    }
}

void StandardMaterial::render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                                   const Skeleton& skeleton,
                                                   const glm::mat4& light_space_matrix,
                                                   const glm::vec3& light_pos, float far_plane) {
    if (begin_shadow_pass(transform, light_space_matrix, &skeleton, true, light_pos, far_plane)) {
        draw_mesh(mesh);
    }
}

// ============================================================================
// Helper methods
// ============================================================================

void StandardMaterial::set_bone_transforms(const Skeleton* skeleton, const Shader& shader) {
    if (skeleton && skeleton->get_bone_count() > 0) {
        auto final_transforms = skeleton->get_final_transforms();
        size_t bone_count = std::min(final_transforms.size(), size_t(200));


        for (size_t i = 0; i < bone_count; ++i) {
            std::string uniform_name = "boneTransforms[" + std::to_string(i) + "]";
            shader.set_mat4(uniform_name, final_transforms[i]);
        }
    }
}

void StandardMaterial::bind_textures() {
    if (albedo_map != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo_map);
        _shader->set_int("albedoMap", 0);
    }

    if (specular_map != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);
        _shader->set_int("specularMap", 1);
    }

    if (metallic_roughness_map != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallic_roughness_map);
        _shader->set_int("metallicRoughnessMap", 2);
    }

    if (normal_map != 0) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normal_map);
        _shader->set_int("normalMap", 3);
    }
}

void StandardMaterial::draw_mesh(const Mesh& mesh) {
    glBindVertexArray(mesh.vao());
    if (mesh.index_count() > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.index_count()), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertex_count()));
    }
    glBindVertexArray(0);
}

}  // namespace engine::pbr
