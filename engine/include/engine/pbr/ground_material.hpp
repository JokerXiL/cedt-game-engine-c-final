#pragma once

#include <engine/pbr/material.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/skeleton.hpp>
#include <engine/pbr/shader.hpp>

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>

namespace engine::pbr {

class Scene;

/// Ground material with pixelated random green color effect
class GroundMaterial : public Material {
public:
    using ShaderLoadFunc = std::function<std::shared_ptr<Shader>(
        const std::string&, const std::string&)>;

    // PBR Material properties
    glm::vec3 albedo;          // Base green color
    glm::vec3 specular_color;
    float metallic;
    float roughness;

    // Pixelation settings
    float pixel_size = 0.5f;         // Size of each "pixel" in world units
    float color_variation = 0.8f;    // Amount of random variation (0-1)

    GroundMaterial(
        ShaderLoadFunc shader_loader,
        const glm::vec3& albedo = glm::vec3(0.3f, 0.6f, 0.3f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float metallic = 0.0f,
        float roughness = 0.8f
    );

    ~GroundMaterial() override = default;

    void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) override;

    void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton, const Scene& scene) override;

    void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                       const glm::mat4& light_space_matrix) override;

    void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                               const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix) override;

    void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                            const glm::mat4& light_space_matrix,
                            const glm::vec3& light_pos, float far_plane) override;

    void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                    const Skeleton& skeleton,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane) override;

private:
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<Shader> _shadow_shader;

    void set_common_uniforms(const glm::mat4& model, const Scene& scene);
    void set_light_uniforms(const Scene& scene);
    void set_shadow_uniforms(const Scene& scene);
    void draw_mesh(const Mesh& mesh);
};

}  // namespace engine::pbr
