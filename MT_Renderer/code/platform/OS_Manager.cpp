//
// Created by Sid on 9/22/2025.
//

#include "OS_Manager.h"
#include <iostream>

namespace platform
{
    void OS_Manager::init_window(WindowCreationParams& creation_params)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW\n");
        }

        window_creation_params = creation_params;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a window
        window = glfwCreateWindow(creation_params.window_width, creation_params.window_height, creation_params.window_title, nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            throw std::runtime_error("Cannot create window");
        }

        glfwMakeContextCurrent(window);
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

    void OS_Manager::os_update()
    {
        // Set viewport and callback
        glViewport(0, 0, window_creation_params.window_width, window_creation_params.window_height);
        //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            //processInput(window);

            // Clear background (dark gray)
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Swap
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }
}
