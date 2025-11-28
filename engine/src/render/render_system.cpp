#include <engine/render/render_system.hpp>
#include <engine/window/window_system.hpp>

#include <glad/glad.h>

namespace engine {
namespace render {

RenderSystem& RenderSystem::get_instance() {
    static RenderSystem instance;
    return instance;
}

void RenderSystem::clear_screen(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::clear_screen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::swap_buffer() {
    auto& window_system = window::WindowSystem::get_instance();
    glfwSwapBuffers(window_system._window);
}

}  // namespace render
}  // namespace engine
