//
// Created by Sid on 9/23/2025.
//

#include "Application.h"

#include <iostream>
#include <ostream>
#include <thread>

namespace core
{
    void Application::application_setup()
    {
        std::cout << "Application Setup" << std::endl;

        ClearState clear_state =
        {
            0.0f, 0.0, 0.5f, 1.0f, 1.0f,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
        };

        renderer_create_clear_state(clear_state);

        RasterStateCreateParams rcp
        {
            .fill_mode = GL_FILL,
            .cull_mode = GL_BACK,
            .front_ccw = GL_CCW,
            .depth_clip_enable = true
        };

        renderer_create_raster_state(rcp);
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

    uint32_t Application::renderer_create_clear_state(const ClearState& clear_state) const
    {
        RenderCommand command{};
        uint32_t resource_slot = render_context->slot_resources.get_next();

        command.command_index = CMD_CREATE_CLEAR_STATE;
        command.command_params = clear_state;
        command.resource_slot = resource_slot;

        add_cmd(command);

        return resource_slot;
    }

    uint32_t Application::renderer_create_raster_state(const RasterStateCreateParams& raster_state_create_params) const
    {
        RenderCommand command{};

        command.command_index = CMD_CREATE_RASTER_STATE;

        command.command_params = raster_state_create_params;

        uint32_t resource_slot = render_context->slot_resources.get_next();
        command.resource_slot = resource_slot;

        add_cmd(command);

        return resource_slot;
    }
} // core