//
// Created by Sid on 9/23/2025.
//

#pragma once
#include "structs/RenderContext.h"

using namespace commands;

namespace core
{
    class Application
    {
    public:
        Application(RenderContext* render_context)
            : render_context(render_context)
        {
        }

        void application_setup();
        void application_update();

        void setup_triangle();
        void add_cmd(const RenderCommand& command) const;

        //Commands
        uint32_t renderer_create_clear_state(const ClearState& clear_state) const;

        uint32_t renderer_create_raster_state(const RasterStateCreateParams& raster_state_create_params) const;


    private:
        RenderContext* render_context;
    };
} // core