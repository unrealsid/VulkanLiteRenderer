#include "core/Application.h"

#include <iostream>
#include <ostream>
#include <thread>

namespace core
{
    void Application::application_setup(const std::function<void(Application*)>& p_application_init_callback,
                                        const std::function<void(Application*)>& p_application_update_callback)
    {
        std::cout << "Application Setup" << std::endl;
        p_application_init_callback(this);
        application_update_callback = p_application_update_callback;

        engine_context->window_manager = std::make_shared<platform::WindowManager>();
        engine_context->window_manager->create_window_glfw(
        {
            window_width, window_height, window_title
        });

        engine_context->start_rendering = true;
    }

    void Application::application_update()
    {
        application_update_callback(this);

        //TODO: Remove later
        std::cout << "Application update started... waiting 4 seconds...." << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::cout << "Application update completed" << std::endl;

        auto window = engine_context->window_manager->get_window();

        while (!glfwWindowShouldClose(window))
        {
            // Input handling
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);

            }

            glfwPollEvents();
        }
    }
} // core