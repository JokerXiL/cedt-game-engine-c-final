#include <engine/window_system.hpp>
#include <iostream>

// Forward declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

namespace engine {

WindowSystem& WindowSystem::get_instance() {
    static WindowSystem window_system;
    return window_system;
}

void WindowSystem::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    this->_window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_NAME, NULL, NULL);

    if (this->_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->_window);
    glfwSetFramebufferSizeCallback(this->_window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

WindowSystem::~WindowSystem() {
    if (_window) {
        glfwSetKeyCallback(_window, nullptr);
        glfwSetCursorPosCallback(_window, nullptr);
        glfwSetMouseButtonCallback(_window, nullptr);
        glfwSetFramebufferSizeCallback(_window, nullptr);

        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    glfwTerminate();
}

bool WindowSystem::should_close() const {
    return glfwWindowShouldClose(_window);
}
void WindowSystem::swap_buffer() { glfwSwapBuffers(_window); }
void WindowSystem::poll_events() { glfwPollEvents(); }
}  // namespace engine

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
