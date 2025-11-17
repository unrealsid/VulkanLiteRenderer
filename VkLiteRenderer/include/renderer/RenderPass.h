#pragma once

#include "Subpass.h"
#include "structs/engine/RenderContext.h"

namespace core::renderer
{
    class RenderPass
    {
    public:
        RenderPass(RenderContext* render_context, std::weak_ptr<EngineContext> engine_context, uint32_t max_frames_in_flight = 2);
        void initialize_geometry_pass();

        std::vector<std::unique_ptr<Subpass>> subpasses;

        [[nodiscard]] uint32_t get_max_frames_in_flight() const { return max_frames_in_flight; }

        void render_pass_init();
        void reset_subpass_command_buffers();

        bool draw_frame();

    private:
        vulkanapp::SwapchainManager* swapchain_manager{};
        vulkanapp::DeviceManager* device_manager{};

        std::vector<VkSemaphore> available_semaphores;
        std::vector<VkSemaphore> finished_semaphores;

        std::vector<VkFence> in_flight_fences;
        std::vector<VkFence> image_in_flight;

        RenderContext* render_context{};
        std::weak_ptr<EngineContext> engine_context;

        uint32_t max_frames_in_flight{};

        size_t current_frame = 0;

        bool create_sync_objects();
    };
}
