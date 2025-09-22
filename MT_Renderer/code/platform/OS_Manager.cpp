//
// Created by Sid on 9/22/2025.
//

#include "OS_Manager.h"
#include <iostream>

namespace platform
{
    void OS_Manager::init_window(WindowCreationParams const& window_creation_params)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW\n");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a window
        window = glfwCreateWindow(window_creation_params.window_width, window_creation_params.window_height, window_creation_params.window_title, nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            throw std::runtime_error("Cannot create window");
        }

        glfwMakeContextCurrent(window);
    }

    void OS_Manager::load_opengl()
    {
        // Load OpenGL functions using GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            glfwTerminate();
            throw std::runtime_error("Cannot load GLAD");
        }
    }

    void OS_Manager::swap_buffers()
    {
        glfwSwapBuffers(window);
    }

    void OS_Manager::poll_events()
    {
        glfwPollEvents();
    }

    void OS_Manager::destroy_window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
