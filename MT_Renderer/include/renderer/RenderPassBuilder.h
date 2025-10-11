#pragma once

#include <memory>

#include "enums/PresentationImageType.h"
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
        explicit RenderPassBuilder(RenderContext* render_context, uint32_t max_frames_in_flight = 2);

        VkCommandBuffer* get_command_buffer(uint32_t pass_id, uint32_t image_id);
        RenderPassBuilder& cache_active_command_buffer(uint32_t pass_id, uint32_t image_id);

        //Init functions
        RenderPassBuilder& init_pass(uint32_t pass_id, bool use_max_frames);
        RenderPassBuilder& create_command_pool();
        RenderPassBuilder& create_depth_stencil_image(uint32_t pass_id);

        //Command Buffer functions
        RenderPassBuilder& allocate_command_buffer(uint32_t pass_id, uint32_t image);
        RenderPassBuilder& begin_command_buffer_recording();
        RenderPassBuilder& set_present_image_transition(uint32_t pass_id, uint32_t image_id, PresentationImageType presentation_image_type);
        RenderPassBuilder& setup_color_attachment(uint32_t image, VkClearValue clear_value);
        RenderPassBuilder& setup_depth_attachment(uint32_t pass_id, VkClearValue clear_value);
        RenderPassBuilder& begin_rendering();
        RenderPassBuilder& record_draw_batches(const std::function<void()>& func);
        RenderPassBuilder& end_command_buffer_recording(uint32_t image);
        
        [[nodiscard]] uint32_t get_max_frames_in_flight() const { return max_frames_in_flight; }

    private:
        RenderContext* render_context;

        uint32_t max_frames_in_flight;
        VkCommandPool command_pool;
        vulkanapp::SwapchainManager* swapchain;

        /*
         *Maps which buffers correspond to which pass_id.
         *Eg if we have max 2 frames from the swapchain and 3 passes (Compute, Render, UI, for eg) the resulting array will look like this -- {0, 0, 1, 1, 2}
         *We can therefore say that the command buffers in 'command_buffers' at indices 0 and 1 belong to pass 0 or the compute buffers
        */
        std::vector<uint32_t> pass_ids;
        std::vector<VkCommandBuffer> command_buffers;

        //Which command buffer are we currently using?
        VkCommandBuffer* active_command_buffer;

        VkRenderingAttachmentInfoKHR color_attachment_info;
        VkRenderingAttachmentInfoKHR depth_attachment_info;

        //Store created Depth Stencils
        std::vector<Vk_Image> depth_stencil_images;
        std::vector<Vk_Image> color_images;

        void draw_frame();

        void cleanup();
    };
}
