#pragma once
#include <cstdint>
#include <functional>
#include "structs/engine/FrameContext.h"


namespace core
{
    class Application
    {
    public:
        explicit Application(FrameContext* render_context)
            : frame_context(render_context)
        {
        }

        void application_setup(const std::function<void(Application*)>& p_application_init_callback,
                               const std::function<void(Application*)>& p_application_update_callback);
        void application_update();

    private:
        FrameContext* frame_context;

        //Update function
        std::function<void(Application*)> application_update_callback;

    };
} // core