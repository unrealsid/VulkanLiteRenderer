#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

#include "structs/engine/WindowCreateParams.h"

struct RenderContext;
struct GLFWwindow;

namespace platform
{
    class WindowManager
    {
    public:
        WindowManager() : window(nullptr){}

        GLFWwindow* create_window_glfw(const WindowCreateParams& window_create_params, bool resize = true);
        VkSurfaceKHR create_surface_glfw(VkInstance instance, VkAllocationCallbacks* allocator = nullptr);
        void destroy_window_glfw() const;
        [[nodiscard]] GLFWwindow* get_window() const;
        VkSurfaceKHR get_surface();

        void update_window_dimensions();
        void setup_window_resize_callback();

        [[nodiscard]] uint32_t get_window_width() const { return m_window_width; }
        [[nodiscard]] uint32_t get_window_height() const { return m_window_height; }

        static void glfw_window_resize_callback(GLFWwindow* glfw_window, int width, int height);

    private:
        GLFWwindow* window;
        VkSurfaceKHR surface;

        uint32_t m_window_width;
        uint32_t m_window_height;
    };
}