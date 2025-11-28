#pragma once

#include <glm/glm.hpp>

namespace engine {
namespace render {

class RenderSystem {
public:
    // Singleton instance
    static RenderSystem& get_instance();

    // Clear the screen with specified color
    void clear_screen(const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Clear with separate RGB and alpha values
    void clear_screen(float r, float g, float b, float a = 1.0f);

    // Swap front and back buffers
    void swap_buffer();

private:
    RenderSystem() = default;
    ~RenderSystem() = default;
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
};

}  // namespace render
}  // namespace engine
