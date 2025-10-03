#pragma once

#include <memory>
#include "platform/WindowManager.h"
#include "structs/engine/FrameContext.h"

struct RenderContext;
struct RenderCommand;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:

        Renderer();

        void renderer_init(FrameContext* p_frame_context);
        void renderer_update();

    private:
        platform::WindowManager* window_manager = nullptr;
        FrameContext* frame_context;
        std::unique_ptr<RenderContext> render_context;

        void init_vulkan();

        void create_window();
        void create_swapchain() const;
        void create_device() const;
        void init_cleanup() const;

        void cleanup();
    };
}
