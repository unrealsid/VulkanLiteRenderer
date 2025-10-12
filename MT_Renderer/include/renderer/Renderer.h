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
        explicit Renderer(uint32_t p_max_frames_in_flight = 2) : pass_builder({}),
                                                                 max_frames_in_flight(p_max_frames_in_flight)
        {
        }

        RenderPassBuilder pass_builder;

        void renderer_init();
        void renderer_update();

        [[nodiscard]] platform::WindowManager* get_window_manager() const
        {
            return window_manager;
        }

        [[nodiscard]] RenderContext* get_render_context() const
        {
            return render_context.get();
        }

    private:
        //TODO: Remove window manager
        platform::WindowManager* window_manager = nullptr;
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
