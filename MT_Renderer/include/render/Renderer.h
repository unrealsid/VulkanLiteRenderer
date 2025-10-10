#pragma once

#include <memory>
#include "platform/WindowManager.h"
#include "structs/engine/RenderContext.h"
#include "structs/engine/FrameContext.h"
#include "structs/vulkan/Vk_Image.h"

struct RenderContext;
struct RenderCommand;
struct WindowCreateParams;

namespace core::renderer
{
    class Renderer
    {
    public:
        Renderer(uint32_t p_max_frames_in_flight = 2) : frame_context(nullptr),
                                                        max_frames_in_flight(p_max_frames_in_flight),
                                                        command_pool(nullptr)
        {
        }

        Renderer(const Renderer&) = default;

        void renderer_init(FrameContext* p_frame_context);
        void renderer_update();

        VkCommandBuffer* get_command_buffer(uint32_t pass_id, uint32_t image_id);
        Renderer& cache_active_command_buffer(uint32_t pass_id, uint32_t image_id);

        //Init functions
        Renderer& init_pass(uint32_t pass_id, bool use_max_frames);
        Renderer& create_command_pool();
        Renderer& create_depth_stencil_image(uint32_t pass_id);

        //Command Buffer functions
        Renderer& allocate_command_buffer(uint32_t pass_id, uint32_t image);
        Renderer& begin_command_buffer_recording();
        Renderer& set_image_layout_transitions_for_active_command_buffer(uint32_t pass_id, uint32_t image);
        Renderer& setup_attachments(uint32_t pass_id, uint32_t image);

        Renderer& record_draw_batches(const std::function<void()>& func);

    private:
        platform::WindowManager* window_manager = nullptr;
        FrameContext* frame_context;
        std::unique_ptr<RenderContext> render_context;

        uint32_t max_frames_in_flight;
        VkCommandPool command_pool;

        std::vector<uint32_t> pass_ids;
        std::vector<VkCommandBuffer> command_buffers;
        VkCommandBuffer* active_command_buffer;

        //Store created Depth Stencils
        std::vector<Vk_Image> depth_stencil_images;
        std::vector<Vk_Image> color_images;

        void init_vulkan();

        void create_window();
        void create_swapchain();
        void create_device() const;
        void init_cleanup() const;

        void draw_frame();

        void cleanup();
    };
}
