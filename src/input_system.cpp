#include "input_system.hpp"
#include <GLFW/glfw3.h>

InputSystem& InputSystem::getInstance() {
    static InputSystem instance;
    return instance;
}

void InputSystem::init(GLFWwindow* window) {
    // Set up GLFW callbacks using lambda functions
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
        InputSystem& input = InputSystem::getInstance();
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                input.setKeyState(key, true);
            else if (action == GLFW_RELEASE)
                input.setKeyState(key, false);
        }
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        InputSystem& input = InputSystem::getInstance();
        input.updateMousePosition(xpos, ypos);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        InputSystem& input = InputSystem::getInstance();
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                input.setRightMousePressed(true);
                input.setFirstMouseMove(true);
            }
            else if (action == GLFW_RELEASE)
            {
                input.setRightMousePressed(false);
            }
        }
    });
}

bool InputSystem::isKeyPressed(int key) const
{
    if (key >= 0 && key < 1024)
        return keys[key];
    return false;
}

void InputSystem::setKeyState(int key, bool pressed)
{
    if (key >= 0 && key < 1024)
        keys[key] = pressed;
}

void InputSystem::updateMousePosition(double xpos, double ypos)
{
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseX = xpos;
    mouseY = ypos;
}
