#include "window_system.hpp"

#include <iostream>

WindowSystem& WindowSystem::get_instance() {
    static WindowSystem window_system;
    return window_system;
}

void WindowSystem::init() {
    this->_window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_NAME, NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(this->_window);
    glfwSetFramebufferSizeCallback(this->_window, framebuffer_size_callback);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
