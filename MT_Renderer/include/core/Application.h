#pragma once
#include <cstdint>
#include <functional>

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

        void application_setup(const std::function<void()>& p_application_init_callback, const std::function<void()>& p_application_update_callback);
        void application_update();

        void add_cmd(const RenderCommand& command) const;

    private:
        FrameContext* render_context;

        //Update function
        std::function<void()> application_update_callback;
    };
} // core