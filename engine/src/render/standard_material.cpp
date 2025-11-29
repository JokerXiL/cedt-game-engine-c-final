#include <engine/render/standard_material.hpp>
#include <engine/render/scene.hpp>
#include <engine/render/shadow_map.hpp>

#include <iostream>
#include <cmath>

namespace engine {

// Static member initialization
GLuint StandardMaterial::_shadow_program_id = 0;
GLuint StandardMaterial::_shadow_cube_program_id = 0;
bool StandardMaterial::_shadow_shaders_initialized = false;

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

    // Create shader program for PBR rendering
    _shader_program_id = create_shader_program("engine/shaders/pbr.vert", "engine/shaders/pbr.frag");

    if (_shader_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Failed to create shader program" << std::endl;
    }
}

void StandardMaterial::initialize_shadow_shaders() {
    if (_shadow_shaders_initialized) return;

    // Create shadow shader for directional/spot lights
    _shadow_program_id = Material::create_shader_program(
        "engine/shaders/shadow.vert",
        "engine/shaders/shadow.frag"
    );

    if (_shadow_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Failed to create shadow shader program" << std::endl;
    }

    // Create shadow cube shader for point lights (multi-pass, no geometry shader)
    _shadow_cube_program_id = Material::create_shader_program(
        "engine/shaders/shadow_cube.vert",
        "engine/shaders/shadow_cube.frag"
    );

    if (_shadow_cube_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Failed to create shadow cube shader program" << std::endl;
    }

    _shadow_shaders_initialized = true;
}

// ============================================================================
// Scene-based rendering (new API)
// ============================================================================

void StandardMaterial::set_light_uniforms(const Scene& scene) {
    const auto& lights = scene.lights();
    int num_lights = static_cast<int>(std::min(lights.size(), size_t(MAX_LIGHTS)));

    set_int(_shader_program_id, "numLights", num_lights);
    set_vec3(_shader_program_id, "ambientColor", scene.ambient());

    for (int i = 0; i < num_lights; ++i) {
        const Light& light = lights[static_cast<size_t>(i)];
        std::string prefix = "lights[" + std::to_string(i) + "].";

        set_int(_shader_program_id, prefix + "type", static_cast<int>(light.type));
        set_vec3(_shader_program_id, prefix + "position", light.position);
        set_vec3(_shader_program_id, prefix + "direction", light.direction);
        set_vec3(_shader_program_id, prefix + "color", light.color);
        set_float(_shader_program_id, prefix + "intensity", light.intensity);
        set_float(_shader_program_id, prefix + "constant", light.attenuation_constant);
        set_float(_shader_program_id, prefix + "linear", light.attenuation_linear);
        set_float(_shader_program_id, prefix + "quadratic", light.attenuation_quadratic);
        // Store cos(angle) for spot lights - shader expects cosine values
        set_float(_shader_program_id, prefix + "innerCutoff", std::cos(light.inner_cutoff));
        set_float(_shader_program_id, prefix + "outerCutoff", std::cos(light.outer_cutoff));
        set_int(_shader_program_id, prefix + "shadowMapIndex", light.shadow_map_index);
    }
}

void StandardMaterial::set_shadow_uniforms(const Scene& scene) {
    auto& shadow_system = ShadowSystem::get_instance();

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

    set_int(_shader_program_id, "numShadowMaps", num_2d_shadows);
    set_int(_shader_program_id, "numShadowCubeMaps", num_cube_shadows);

    // Set light space matrices for 2D shadow maps
    for (int i = 0; i < num_2d_shadows; ++i) {
        auto* shadow_map = shadow_system.get_shadow_map(i);
        if (shadow_map) {
            std::string name = "lightSpaceMatrices[" + std::to_string(i) + "]";
            set_mat4(_shader_program_id, name, shadow_map->light_space_matrix);
        }
    }

    // Set point light far planes for cubemap shadows
    for (int i = 0; i < num_cube_shadows; ++i) {
        auto* cubemap = shadow_system.get_shadow_cubemap(i);
        if (cubemap) {
            std::string name = "pointLightFarPlanes[" + std::to_string(i) + "]";
            set_float(_shader_program_id, name, cubemap->far_plane);
        }
    }

    // Bind shadow texture samplers
    int shadow_unit = shadow_system.shadow_map_texture_unit();
    for (int i = 0; i < MAX_SHADOW_MAPS; ++i) {
        std::string name = "shadowMaps[" + std::to_string(i) + "]";
        set_int(_shader_program_id, name, shadow_unit + i);
    }

    int cubemap_unit = shadow_system.shadow_cubemap_texture_unit();
    for (int i = 0; i < MAX_SHADOW_CUBEMAPS; ++i) {
        std::string name = "shadowCubeMaps[" + std::to_string(i) + "]";
        set_int(_shader_program_id, name, cubemap_unit + i);
    }

    // Bind shadow textures
    shadow_system.bind_shadow_textures();
}

void StandardMaterial::set_common_uniforms(const glm::mat4& model, const Scene& scene) {
    use_program(_shader_program_id);

    const Camera* camera = scene.camera();
    if (!camera) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Scene has no camera" << std::endl;
        return;
    }

    // Transformation matrices
    set_mat4(_shader_program_id, "model", model);
    set_mat4(_shader_program_id, "view", camera->view());
    set_mat4(_shader_program_id, "projection", camera->projection());

    // View position
    set_vec3(_shader_program_id, "viewPos", camera->position());

    // Material properties
    set_vec3(_shader_program_id, "albedo", albedo);
    set_vec3(_shader_program_id, "specularColor", specular_color);
    set_float(_shader_program_id, "metallic", metallic);
    set_float(_shader_program_id, "roughness", roughness);

    // Texture flags
    set_bool(_shader_program_id, "useAlbedoMap", albedo_map != 0);
    set_bool(_shader_program_id, "useSpecularMap", specular_map != 0);
    set_bool(_shader_program_id, "useMetallicMap", metallic_roughness_map != 0);
    set_bool(_shader_program_id, "useRoughnessMap", metallic_roughness_map != 0);
    set_bool(_shader_program_id, "useNormalMap", normal_map != 0);

    // Lighting
    set_light_uniforms(scene);

    // Shadow maps
    set_shadow_uniforms(scene);

    // Bind material textures
    bind_textures();
}

void StandardMaterial::render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) {
    if (_shader_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms(transform, scene);
    set_bool(_shader_program_id, "useSkinning", false);
    draw_mesh(mesh);
}

void StandardMaterial::render_skinned(const Mesh& mesh, const glm::mat4& transform,
                                       const Skeleton& skeleton, const Scene& scene) {
    if (_shader_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms(transform, scene);
    set_bool(_shader_program_id, "useSkinning", true);
    set_bone_transforms(&skeleton);
    draw_mesh(mesh);
}

// ============================================================================
// Shadow rendering
// ============================================================================

void StandardMaterial::render_shadow(const Mesh& mesh, const glm::mat4& transform,
                                      const glm::mat4& light_space_matrix) {
    if (_shadow_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Shadow shader not initialized" << std::endl;
        return;
    }

    use_program(_shadow_program_id);
    set_mat4(_shadow_program_id, "model", transform);
    set_mat4(_shadow_program_id, "lightSpaceMatrix", light_space_matrix);
    set_bool(_shadow_program_id, "useSkinning", false);
    draw_mesh(mesh);
}

void StandardMaterial::render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                                              const Skeleton& skeleton,
                                              const glm::mat4& light_space_matrix) {
    if (_shadow_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Shadow shader not initialized" << std::endl;
        return;
    }

    use_program(_shadow_program_id);
    set_mat4(_shadow_program_id, "model", transform);
    set_mat4(_shadow_program_id, "lightSpaceMatrix", light_space_matrix);
    set_bool(_shadow_program_id, "useSkinning", true);
    set_bone_transforms(&skeleton, _shadow_program_id);
    draw_mesh(mesh);
}

void StandardMaterial::render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                                           const glm::mat4& light_space_matrix,
                                           const glm::vec3& light_pos, float far_plane) {
    if (_shadow_cube_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Shadow cube shader not initialized" << std::endl;
        return;
    }

    use_program(_shadow_cube_program_id);
    set_mat4(_shadow_cube_program_id, "model", transform);
    set_mat4(_shadow_cube_program_id, "lightSpaceMatrix", light_space_matrix);
    set_vec3(_shadow_cube_program_id, "lightPos", light_pos);
    set_float(_shadow_cube_program_id, "farPlane", far_plane);
    set_bool(_shadow_cube_program_id, "useSkinning", false);
    draw_mesh(mesh);
}

void StandardMaterial::render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                                   const Skeleton& skeleton,
                                                   const glm::mat4& light_space_matrix,
                                                   const glm::vec3& light_pos, float far_plane) {
    if (_shadow_cube_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Shadow cube shader not initialized" << std::endl;
        return;
    }

    use_program(_shadow_cube_program_id);
    set_mat4(_shadow_cube_program_id, "model", transform);
    set_mat4(_shadow_cube_program_id, "lightSpaceMatrix", light_space_matrix);
    set_vec3(_shadow_cube_program_id, "lightPos", light_pos);
    set_float(_shadow_cube_program_id, "farPlane", far_plane);
    set_bool(_shadow_cube_program_id, "useSkinning", true);
    set_bone_transforms(&skeleton, _shadow_cube_program_id);
    draw_mesh(mesh);
}

// ============================================================================
// Legacy rendering (backwards compatibility)
// ============================================================================

void StandardMaterial::set_common_uniforms_legacy(const glm::mat4& model) {
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

    // Legacy single light - convert to new format
    set_int(_shader_program_id, "numLights", 1);
    set_vec3(_shader_program_id, "ambientColor", glm::vec3(0.1f));

    // Set as a point light
    set_int(_shader_program_id, "lights[0].type", 1);  // Point
    set_vec3(_shader_program_id, "lights[0].position", light_pos);
    set_vec3(_shader_program_id, "lights[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
    set_vec3(_shader_program_id, "lights[0].color", light_color);
    set_float(_shader_program_id, "lights[0].intensity", 1.0f);
    set_float(_shader_program_id, "lights[0].constant", 1.0f);
    set_float(_shader_program_id, "lights[0].linear", 0.0f);
    set_float(_shader_program_id, "lights[0].quadratic", 0.0f);
    set_float(_shader_program_id, "lights[0].innerCutoff", 1.0f);
    set_float(_shader_program_id, "lights[0].outerCutoff", 0.9f);
    set_int(_shader_program_id, "lights[0].shadowMapIndex", -1);

    // No shadows in legacy mode
    set_int(_shader_program_id, "numShadowMaps", 0);
    set_int(_shader_program_id, "numShadowCubeMaps", 0);

    // View position
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

void StandardMaterial::render(const Mesh& mesh, const glm::mat4& transform) {
    if (_shader_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms_legacy(transform);
    set_bool(_shader_program_id, "useSkinning", false);
    draw_mesh(mesh);
}

void StandardMaterial::render_skinned(const Mesh& mesh, const glm::mat4& transform,
                                       const Skeleton& skeleton) {
    if (_shader_program_id == 0) {
        std::cerr << "ERROR::STANDARD_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms_legacy(transform);
    set_bool(_shader_program_id, "useSkinning", true);
    set_bone_transforms(&skeleton);
    draw_mesh(mesh);
}

// ============================================================================
// Helper methods
// ============================================================================

void StandardMaterial::set_bone_transforms(const Skeleton* skeleton) {
    set_bone_transforms(skeleton, _shader_program_id);
}

void StandardMaterial::set_bone_transforms(const Skeleton* skeleton, GLuint program_id) {
    if (skeleton && skeleton->get_bone_count() > 0) {
        auto final_transforms = skeleton->get_final_transforms();
        size_t bone_count = std::min(final_transforms.size(), size_t(100));

        for (size_t i = 0; i < bone_count; ++i) {
            std::string uniform_name = "boneTransforms[" + std::to_string(i) + "]";
            set_mat4(program_id, uniform_name, final_transforms[i]);
        }
    }
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

void StandardMaterial::draw_mesh(const Mesh& mesh) {
    glBindVertexArray(mesh.vao());
    if (mesh.index_count() > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.index_count()), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertex_count()));
    }
    glBindVertexArray(0);
}

}  // namespace engine
