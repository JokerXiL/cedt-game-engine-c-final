#include "input_system.hpp"

#include <GLFW/glfw3.h>

#include "window_system.hpp"

InputSystem& InputSystem::get_instance() {
    static InputSystem instance;
    return instance;
}

void InputSystem::init() {
    auto* window = WindowSystem::get_instance().window();

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
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if (action == GLFW_PRESS) {
                    input.set_right_mouse_pressed(true);
                    input.set_first_mouse_move(true);
                } else if (action == GLFW_RELEASE) {
                    input.set_right_mouse_pressed(false);
                }
            }
        });
}