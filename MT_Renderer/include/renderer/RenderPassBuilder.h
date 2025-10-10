#pragma once

#include <memory>
#include "platform/WindowManager.h"
#include "structs/engine/RenderContext.h"
#include "structs/vulkan/Vk_Image.h"

struct RenderContext;
struct RenderCommand;
struct WindowCreateParams;

namespace core::renderer
{
    class RenderPassBuilder
    {
    public:
        RenderPassBuilder(uint32_t p_max_frames_in_flight = 2) : max_frames_in_flight(0), command_pool(nullptr),
                                                        active_command_buffer(nullptr)
        {
        }

        VkCommandBuffer* get_command_buffer(uint32_t pass_id, uint32_t image_id);
        RenderPassBuilder& cache_active_command_buffer(uint32_t pass_id, uint32_t image_id);

        //Init functions
        RenderPassBuilder& init_pass(uint32_t pass_id, bool use_max_frames);
        RenderPassBuilder& create_command_pool();
        RenderPassBuilder& create_depth_stencil_image(uint32_t pass_id);

        //Command Buffer functions
        RenderPassBuilder& allocate_command_buffer(uint32_t pass_id, uint32_t image);
        RenderPassBuilder& begin_command_buffer_recording();
        RenderPassBuilder& set_image_layout_transitions_for_active_command_buffer(uint32_t pass_id, uint32_t image);
        RenderPassBuilder& setup_attachments(uint32_t pass_id, uint32_t image);

        RenderPassBuilder& record_draw_batches(const std::function<void()>& func);

        RenderPassBuilder& end_command_buffer_recording(uint32_t image);

    private:
        RenderContext* render_context;

        uint32_t max_frames_in_flight;
        VkCommandPool command_pool;

        std::vector<uint32_t> pass_ids;
        std::vector<VkCommandBuffer> command_buffers;
        VkCommandBuffer* active_command_buffer;

        //Store created Depth Stencils
        std::vector<Vk_Image> depth_stencil_images;
        std::vector<Vk_Image> color_images;
        void draw_frame();

        void cleanup();
    };
}
