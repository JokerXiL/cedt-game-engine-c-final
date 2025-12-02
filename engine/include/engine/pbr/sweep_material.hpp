#pragma once

#include <engine/pbr/material.hpp>
#include <engine/pbr/shader.hpp>

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <string>

namespace engine::pbr {

class Scene;
class Mesh;

/// Material for animated sweep effects (sword slashes, etc.)
/// Uses additive blending and is unlit (ignores scene lighting)
class SweepMaterial : public Material {
public:
    using ShaderLoadFunc = std::function<std::shared_ptr<Shader>(
        const std::string&, const std::string&)>;

    // Effect parameters
    glm::vec3 color{1.0f, 0.5f, 0.2f};  // Orange-ish default
    float alpha = 0.8f;
    float sweep_progress = 0.0f;         // 0.0 to 1.0
    float trail_length = 0.4f;           // Length of fading trail

    explicit SweepMaterial(ShaderLoadFunc shader_loader);
    ~SweepMaterial() override = default;

    void render(const Mesh& mesh, const glm::mat4& transform, const Scene& scene) override;

    // Not used for sweep effects (no shadows)
    void render_skinned(const Mesh& mesh, const glm::mat4& transform,
                        const Skeleton& skeleton, const Scene& scene) override {}
    void render_shadow(const Mesh& mesh, const glm::mat4& transform,
                       const glm::mat4& light_space_matrix) override {}
    void render_shadow_skinned(const Mesh& mesh, const glm::mat4& transform,
                               const Skeleton& skeleton,
                               const glm::mat4& light_space_matrix) override {}
    void render_shadow_cube(const Mesh& mesh, const glm::mat4& transform,
                            const glm::mat4& light_space_matrix,
                            const glm::vec3& light_pos, float far_plane) override {}
    void render_shadow_cube_skinned(const Mesh& mesh, const glm::mat4& transform,
                                    const Skeleton& skeleton,
                                    const glm::mat4& light_space_matrix,
                                    const glm::vec3& light_pos, float far_plane) override {}

private:
    std::shared_ptr<Shader> _shader;
};

}  // namespace engine::pbr
