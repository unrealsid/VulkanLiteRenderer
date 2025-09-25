#pragma once
#include <memory>

#include "platform/OS_Manager.h"
#include "structs/RenderContext.h"

struct WindowCreationParams;

namespace core
{
    class Renderer
    {
    public:
        void renderer_init(RenderContext* p_render_context, platform::OS_Manager* p_os_manager);
        void renderer_update( RenderContext* p_render_context);

    private:
        static void load_opengl();
        platform::OS_Manager* os_manager = nullptr;
    };
}
