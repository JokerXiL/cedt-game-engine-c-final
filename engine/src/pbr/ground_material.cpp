#include <engine/pbr/ground_material.hpp>
#include <engine/pbr/scene.hpp>

#include <iostream>
#include <cmath>

namespace engine::pbr {

GroundMaterial::GroundMaterial(
    ShaderLoadFunc shader_loader,
    const glm::vec3& albedo,
    const glm::vec3& specular,
    float metallic,
    float roughness
)
    : albedo(albedo),
      specular_color(specular),
      metallic(metallic),
      roughness(roughness) {

    // Load ground shader with pixelation effect
    _shader = shader_loader("engine/shaders/ground.vert", "engine/shaders/ground.frag");
    _shadow_shader = shader_loader("engine/shaders/shadow.vert", "engine/shaders/shadow.frag");

    if (!_shader || !_shader->valid()) {
        std::cerr << "ERROR::GROUND_MATERIAL::Failed to load ground shader" << std::endl;
    }
    if (!_shadow_shader || !_shadow_shader->valid()) {
        std::cerr << "ERROR::GROUND_MATERIAL::Failed to load shadow shader" << std::endl;
    }
}

void GroundMaterial::set_light_uniforms(const Scene& scene) {
    const auto& lights = scene.lights();
    int num_lights = static_cast<int>(std::min(lights.size(), size_t(MAX_LIGHTS)));

    _shader->set_int("numLights", num_lights);
    _shader->set_vec3("ambient", scene.ambient());

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
        _shader->set_float(prefix + "cutoff", std::cos(light.inner_cutoff));
        _shader->set_float(prefix + "outerCutoff", std::cos(light.outer_cutoff));
        _shader->set_int(prefix + "shadowMapIndex", light.shadow_map_index);
    }
}

void GroundMaterial::set_shadow_uniforms(const Scene& scene) {
    const ShadowData* shadow_data = scene.shadow_data();
    if (!shadow_data) return;

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

void GroundMaterial::set_common_uniforms(const glm::mat4& model, const Scene& scene) {
    _shader->use();

    const Camera* camera = scene.camera();
    if (!camera) {
        std::cerr << "ERROR::GROUND_MATERIAL::Scene has no camera" << std::endl;
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
    _shader->set_vec3("specular", specular_color);
    _shader->set_float("metallic", metallic);
    _shader->set_float("roughness", roughness);

    // Pixelation settings
    _shader->set_float("pixelSize", pixel_size);
    _shader->set_float("colorVariation", color_variation);
    _shader->set_float("time", 0.0f);  // Static for now

    // Lighting
    set_light_uniforms(scene);

    // Shadow maps
    set_shadow_uniforms(scene);
}

void GroundMaterial::render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) {
    if (!_shader || !_shader->valid()) {
        std::cerr << "ERROR::GROUND_MATERIAL::Invalid shader program" << std::endl;
        return;
    }

    set_common_uniforms(transform, scene);
    draw_mesh(mesh);
}

void GroundMaterial::render_skinned(const Mesh& mesh, const glm::mat4& transform,
                                     const Skeleton& skeleton, const Scene& scene) {
    // Ground doesn't support skinning, just render normally
    render(mesh, transform, scene);
}

void GroundMaterial::render_shadow(const Mesh& mesh, const glm::mat4& transform,
                                    const glm::mat4& light_space_matrix) {
    if (!_shadow_shader || !_shadow_shader->valid()) return;

    _shadow_shader->use();
    _shadow_shader->set_mat4("model", transform);
    _shadow_shader->set_mat4("lightSpaceMatrix", light_space_matrix);
    _shadow_shader->set_bool("useSkinning", false);
    _shadow_shader->set_bool("isPointLight", false);
    draw_mesh(mesh);
}

void GroundMaterial::render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                                            const Skeleton& skeleton,
                                            const glm::mat4& light_space_matrix) {
    render_shadow(mesh, transform, light_space_matrix);
}

void GroundMaterial::render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                                         const glm::mat4& light_space_matrix,
                                         const glm::vec3& light_pos, float far_plane) {
    if (!_shadow_shader || !_shadow_shader->valid()) return;

    _shadow_shader->use();
    _shadow_shader->set_mat4("model", transform);
    _shadow_shader->set_mat4("lightSpaceMatrix", light_space_matrix);
    _shadow_shader->set_bool("useSkinning", false);
    _shadow_shader->set_bool("isPointLight", true);
    _shadow_shader->set_vec3("lightPos", light_pos);
    _shadow_shader->set_float("farPlane", far_plane);
    draw_mesh(mesh);
}

void GroundMaterial::render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                                 const Skeleton& skeleton,
                                                 const glm::mat4& light_space_matrix,
                                                 const glm::vec3& light_pos, float far_plane) {
    render_shadow_cube(mesh, transform, light_space_matrix, light_pos, far_plane);
}

void GroundMaterial::draw_mesh(const Mesh& mesh) {
    glBindVertexArray(mesh.vao());
    if (mesh.index_count() > 0) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.index_count()), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertex_count()));
    }
    glBindVertexArray(0);
}

}  // namespace engine::pbr
