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
}

namespace game::main_game {

/// Material for animated sweep effects (sword slashes, etc.)
/// Uses additive blending and is unlit (ignores scene lighting)
class SweepMaterial : public engine::pbr::Material {
public:
    using ShaderLoadFunc = std::function<std::shared_ptr<engine::pbr::Shader>(
        const std::string&, const std::string&)>;

    // Effect parameters
    glm::vec3 color{1.0f, 0.5f, 0.2f};  // Orange-ish default
    float alpha = 0.8f;
    float sweep_progress = 0.0f;         // 0.0 to 1.0
    float trail_length = 0.4f;           // Length of fading trail

    explicit SweepMaterial(ShaderLoadFunc shader_loader);
    ~SweepMaterial() override = default;

    void render(const engine::pbr::Mesh& mesh, const glm::mat4& transform,
                const engine::pbr::Scene& scene) override;

private:
    std::shared_ptr<engine::pbr::Shader> _shader;
};

}  // namespace game::main_game
