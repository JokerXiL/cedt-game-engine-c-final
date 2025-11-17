#pragma once

#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const char* WINDOW_NAME = "Game";

class WindowSystem {
public:
    // Singleton instance
    static WindowSystem& get_instance();

    void init();

    GLFWwindow* window() const { return this->_window; }

private:
    GLFWwindow* _window;
};