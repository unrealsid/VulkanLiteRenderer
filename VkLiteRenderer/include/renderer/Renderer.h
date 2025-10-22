#pragma once

#include <memory>

#include "RenderPassBuilder.h"
#include "platform/WindowManager.h"
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
        explicit Renderer(uint32_t p_max_frames_in_flight = 2) : max_frames_in_flight(p_max_frames_in_flight){}

        std::unique_ptr<RenderPassBuilder> pass_builder;

        void renderer_init();
        void renderer_update();

        [[nodiscard]] RenderContext* get_render_context() const
        {
            return render_context.get();
        }

    private:
        std::unique_ptr<RenderContext> render_context;

        uint32_t max_frames_in_flight;

        void init_vulkan();

        void create_window();
        void create_swapchain();
        void create_device() const;
        void init_cleanup() const;

        void cleanup();
    };
}
