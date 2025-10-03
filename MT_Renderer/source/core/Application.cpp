#include "core/Application.h"

namespace core
{
    void Application::application_setup()
    {

    }

    void Application::application_update()
    {

    }

    void Application::setup_triangle()
    {

    }

    void Application::add_cmd(const RenderCommand& command) const
    {
        render_context->render_command_buffer.put(command);
    }
} // core