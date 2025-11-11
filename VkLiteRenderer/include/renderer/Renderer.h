#pragma once

#include <memory>

#include "RenderPass.h"
#include "Subpass.h"
#include "platform/WindowManager.h"
#include "structs/engine/FrameContext.h"
#include "structs/engine/RenderContext.h"
#include "structs/vulkan/Vk_Image.h"

struct RenderContext;
struct RenderCommand;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:
        explicit Renderer(FrameContext* frame_context, uint32_t p_max_frames_in_flight = 2) : frame_context(frame_context),
                                                                                              max_frames_in_flight(p_max_frames_in_flight)
        {
        }

        void renderer_init();
        void renderer_update() const;

        [[nodiscard]] RenderContext* get_render_context() const
        {
            return render_context.get();
        }

    private:
        std::unique_ptr<RenderPass> render_pass;
        std::unique_ptr<RenderContext> render_context;
        FrameContext* frame_context;

        uint32_t max_frames_in_flight;

        void init_vulkan();

        void create_swapchain();
        void create_device() const;
        void init_cleanup() const;

        void cleanup();
    };
}
