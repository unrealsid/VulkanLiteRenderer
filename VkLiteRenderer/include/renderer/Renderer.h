#pragma once

#include <memory>
#include <utility>

#include "RenderPass.h"
#include "Subpass.h"
#include "platform/WindowManager.h"
#include "structs/engine/EngineContext.h"
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
        explicit Renderer(std::shared_ptr<EngineContext> engine_context, const uint32_t p_max_frames_in_flight = 2) :
                                                                        engine_context(std::move(engine_context)),
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

        std::shared_ptr<EngineContext> engine_context;

        //How many images are we using for a single frame?
        uint32_t max_frames_in_flight;

        void init_vulkan();

        void create_swapchain() const;
        void create_device() const;
        void init_cleanup() const;

        void cleanup();
    };
}
