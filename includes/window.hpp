#pragma once

#include <GLFW/glfw3.h>

class Window {
    GLFWwindow* window;

public:
    static Window initialize() {
    //     {
    //     glfwInit();
    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // #ifdef __APPLE__
    //     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // #endif

    //     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tank Game - OpenGL", NULL, NULL);
    //     if (window == NULL)
    //     {
    //         std::cout << "Failed to create GLFW window" << std::endl;
    //         glfwTerminate();
    //         return -1;
    //     }
    //     glfwMakeContextCurrent(window);
    //     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //     glfwSetKeyCallback(window, key_callback);
    //     glfwSetCursorPosCallback(window, mouse_callback);
    //     glfwSetMouseButtonCallback(window, mouse_button_callback);

    //     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //     {
    //         std::cout << "Failed to initialize GLAD" << std::endl;
    //         return -1;
    //     }

    //     glEnable(GL_DEPTH_TEST);
    // }
    }
};