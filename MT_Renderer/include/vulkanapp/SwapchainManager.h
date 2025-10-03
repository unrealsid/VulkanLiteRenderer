#pragma once

#include <VkBootstrap.h>

struct RenderContext;

//Manages the swapchain
namespace vulkanapp
{
    class SwapchainManager
    {
    public:

        SwapchainManager(RenderContext& engine_context) : render_context(engine_context){}
        ~SwapchainManager();
        
        bool create_swapchain();
        bool recreate_swapchain();
        void cleanup();

        [[nodiscard]] vkb::Swapchain get_swapchain() const { return swapchain; }
        [[nodiscard]] const std::vector<VkImage>& get_swapchain_images() const { return swapchain_images; }
        [[nodiscard]] const std::vector<VkImageView>& get_swapchain_image_views() const { return swapchain_image_views; }
        [[nodiscard]] VkFormat get_swapchain_format() const { return swapchain.image_format; }
        [[nodiscard]] VkExtent2D get_swapchain_extent() const { return swapchain.extent; }

    private:
        RenderContext& render_context;

        vkb::Swapchain swapchain{};
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        
        void cleanup_image_views();
        void cleanup_images();
    };
}