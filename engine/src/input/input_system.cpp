#include <engine/input/input_system.hpp>

#include <engine/window/window_system.hpp>

namespace engine {
namespace input {

InputSystem& InputSystem::get_instance() {
    static InputSystem instance;
    return instance;
}

void InputSystem::init() {
    auto* window = window::WindowSystem::get_instance()._window;

    // Set up GLFW callbacks using lambda functions
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                  int action, int mode) {
        InputSystem& input = InputSystem::get_instance();
        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS)
                input.set_key_state(key, true);
            else if (action == GLFW_RELEASE)
                input.set_key_state(key, false);
        }
    });

    glfwSetCursorPosCallback(
        window, [](GLFWwindow* window, double xpos, double ypos) {
            InputSystem& input = InputSystem::get_instance();
            input.update_mouse_position(xpos, ypos);
        });

    glfwSetMouseButtonCallback(
        window, [](GLFWwindow* window, int button, int action, int mods) {
            InputSystem& input = InputSystem::get_instance();
            // TODO
        });
}

void InputSystem::capture_mouse() {
    auto* window = window::WindowSystem::get_instance()._window;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputSystem::release_mouse() {
    auto* window = window::WindowSystem::get_instance()._window;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void InputSystem::get_mouse_delta(float& dx, float& dy) {
    dx = static_cast<float>(_mouse_x - _last_mouse_x);
    dy = static_cast<float>(_mouse_y - _last_mouse_y);
    _last_mouse_x = _mouse_x;
    _last_mouse_y = _mouse_y;
}

void InputSystem::end_frame() {
    for (int i = 0; i < 1024; ++i) {
        _keys_just_pressed[i] = false;
    }
}

}  // namespace input
}  // namespace engine
