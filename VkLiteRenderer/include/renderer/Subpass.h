#pragma once

#include <memory>
#include "enums/PresentationImageType.h"
#include "materials/Material.h"
#include "platform/WindowManager.h"
#include "structs/engine/RenderContext.h"
#include "structs/vulkan/Vk_Image.h"

struct RenderContext;
struct RenderCommand;
struct WindowCreateParams;

namespace core::renderer
{
    class Subpass
    {
    public:
        explicit Subpass(RenderContext* render_context, uint32_t max_frames_in_flight = 2);

        //Gets the command buffer at the image ID when using double/triple buffering
        VkCommandBuffer* get_command_buffer(uint32_t image_id);
        Subpass& cache_active_command_buffer(uint32_t image_id);

        //Init functions
        Subpass& init_pass(bool use_max_frames);
        Subpass& create_command_pool();
        Subpass& create_depth_stencil_image();

        //Command Buffer functions
        Subpass& allocate_command_buffer(uint32_t image);
        Subpass& begin_command_buffer_recording();
        Subpass& set_present_image_transition(uint32_t image_id, PresentationImageType presentation_image_type);
        Subpass& setup_color_attachment(uint32_t image, VkClearValue clear_value);
        Subpass& setup_depth_attachment(VkClearValue clear_value);
        Subpass& begin_rendering();
        Subpass& end_rendering();
        Subpass& record_draw_batches(const std::function<void(VkCommandBuffer current_buffer,
            RenderContext* render_context,
            material::Material* material)>& func);
        Subpass& end_command_buffer_recording(uint32_t image);

        Subpass& set_material(material::Material& material);

    private:
        RenderContext* render_context;

        VkCommandPool command_pool;
        vulkanapp::SwapchainManager* swapchain_manager;
        vulkanapp::DeviceManager* device_manager;

        //Command Buffers are indexed by immage_id assuming we have a frame buffering
        std::vector<VkCommandBuffer> command_buffers;

        uint32_t max_frames_in_flight;

        //Which command buffer are we currently using?
        VkCommandBuffer* active_command_buffer;

        VkRenderingAttachmentInfoKHR color_attachment_info;
        VkRenderingAttachmentInfoKHR depth_attachment_info;

        //Store created Depth Stencils
        Vk_Image depth_stencil_image;
        Vk_Image color_images;

        material::Material* material_to_use;

        void cleanup();
    };
}
