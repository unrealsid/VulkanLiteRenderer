#include "platform/OS_Manager.h"
#include <iostream>

namespace platform
{
    void OS_Manager::init_window(WindowCreateParams& creation_params)
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
        //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            //processInput(window);



            // Swap
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }
}
