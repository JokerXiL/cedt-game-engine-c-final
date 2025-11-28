#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {

// Forward declarations for friends
namespace ui {
class UISystem;
}

namespace input {
class InputSystem;
}

namespace render {
class RenderSystem;
}

namespace window {

constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;
inline const char* WINDOW_NAME = "Game";

class WindowSystem {
    friend class engine::ui::UISystem;
    friend class engine::input::InputSystem;
    friend class engine::render::RenderSystem;

public:
    // Singleton instance
    static WindowSystem& get_instance();

    void init();
    ~WindowSystem();

    bool should_close() const;
    void poll_events();
    double get_time() const;

private:
    WindowSystem() = default;
    WindowSystem(const WindowSystem&) = delete;
    WindowSystem& operator=(const WindowSystem&) = delete;

    GLFWwindow* _window = nullptr;
};

}  // namespace window
}  // namespace engine
