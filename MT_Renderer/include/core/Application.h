#pragma once
#include <cstdint>
#include "structs/engine/FrameContext.h"

using namespace commands;

namespace core
{
    class Application
    {
    public:
        explicit Application(FrameContext* render_context)
            : render_context(render_context)
        {
        }

        void application_setup();
        void application_update();

        void setup_triangle();
        void add_cmd(const RenderCommand& command) const;


    private:
        FrameContext* render_context;
    };
} // core