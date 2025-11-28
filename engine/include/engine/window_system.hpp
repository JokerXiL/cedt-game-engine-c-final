#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {

constexpr unsigned int SCR_WIDTH = 1280;
constexpr unsigned int SCR_HEIGHT = 720;
inline const char* WINDOW_NAME = "Game";

class WindowSystem {
public:
    // Singleton instance
    static WindowSystem& get_instance();

    void init();

    GLFWwindow* window() const { return this->_window; }

private:
    GLFWwindow* _window;
};

}  // namespace engine