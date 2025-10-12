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

    private:
        GLFWwindow* window;
        VkSurfaceKHR surface;
    };
}