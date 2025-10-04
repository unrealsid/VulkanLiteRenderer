#include "core/Application.h"

#include <iostream>
#include <ostream>
#include <thread>

namespace core
{
    void Application::application_setup(const std::function<void()>& p_application_init_callback,
                                        const std::function<void()>& p_application_update_callback)
    {
        std::cout << "Application Setup" << std::endl;
        p_application_init_callback();
        application_update_callback = p_application_update_callback;

        application_update();
    }

    void Application::application_update()
    {
        application_update_callback();

        std::cout << "Application update started... waiting 4 seconds...." << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(4000));

        std::cout << "Application update completed" << std::endl;
    }

    void Application::add_cmd(const RenderCommand& command) const
    {
        render_context->render_command_buffer.try_enqueue(command);
    }
} // core