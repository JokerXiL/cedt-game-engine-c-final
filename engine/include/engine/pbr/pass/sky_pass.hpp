#pragma once

#include <engine/pbr/pass/pbr_render_pass.hpp>
#include <engine/pbr/shader.hpp>

#include <memory>

namespace engine::pbr {

/// Sky rendering pass
/// Writes: color_buffer (background, should render before PBR objects)
class SkyPass : public PBRRenderPass {
public:
    using ShaderLoadFunc = std::function<std::shared_ptr<Shader>(const std::string&, const std::string&)>;

    explicit SkyPass(ShaderLoadFunc shader_loader);

    void execute() override;

    // Sky configuration
    void set_sky_colors(const glm::vec3& top, const glm::vec3& horizon) {
        _sky_color_top = top;
        _sky_color_horizon = horizon;
    }

    void set_sun(const glm::vec3& direction, const glm::vec3& color, float size = 0.9998f) {
        _sun_direction = glm::normalize(direction);
        _sun_color = color;
        _sun_size = size;
    }

private:
    std::shared_ptr<Shader> _shader;
    GLuint _vao = 0;  // Empty VAO for fullscreen triangle

    // Sky settings
    glm::vec3 _sky_color_top{0.4f, 0.6f, 0.9f};      // Blue
    glm::vec3 _sky_color_horizon{0.8f, 0.85f, 0.9f}; // Light blue/white
    glm::vec3 _sun_direction{0.5f, 0.3f, 0.8f};      // Sun direction
    glm::vec3 _sun_color{1.0f, 0.95f, 0.85f};        // Warm white
    float _sun_size = 0.9998f;                        // Cosine of sun angular size
};

}  // namespace engine::pbr
