#pragma once
#include <cstdint>
#include <functional>

#include "structs/engine/FrameContext.h"

struct ClearState;
using namespace commands;

namespace core
{
    class Application
    {
    public:
        explicit Application(FrameContext* render_context)
            : frame_context(render_context)
        {
        }

        void application_setup(const std::function<void(Application*)>& p_application_init_callback, const std::function<void(Application*)>& p_application_update_callback);
        void application_update();

        //Queue Filling Functions
        [[nodiscard]] uint32_t renderer_create_clear_state(const ClearState& cs) const;

        uint32_t clear_state_resource = 0;

    private:
        FrameContext* frame_context;

        //Update function
        std::function<void(Application*)> application_update_callback;

        //Adds a command to the queue
        void add_cmd(const RenderCommand& command) const;
    };
} // core