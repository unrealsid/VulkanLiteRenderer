#include "platform/WindowManager.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "Config.inl"

namespace platform
{
    GLFWwindow* WindowManager::create_window_glfw(const WindowCreateParams& window_create_params, bool resize)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        if (!resize)
        {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }

        window = glfwCreateWindow(window_create_params.window_width, window_create_params.window_height,
                                  window_create_params.window_title, nullptr, nullptr);
        return window;
    }

    void WindowManager::destroy_window_glfw() const
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    VkSurfaceKHR WindowManager::create_surface_glfw(VkInstance instance, VkAllocationCallbacks* allocator)
    {
        surface = VK_NULL_HANDLE;
        VkResult err = glfwCreateWindowSurface(instance, window, allocator, &surface);

        if (err)
        {
            const char* error_msg;
            int ret = glfwGetError(&error_msg);

            if (ret != 0)
            {
                std::cout << ret << " ";
                if (error_msg != nullptr) std::cout << error_msg;
                std::cout << "\n";
            }
            surface = VK_NULL_HANDLE;
        }
        return surface;
    }

    GLFWwindow* WindowManager::get_window() const
    {
        return window;
    }

    VkSurfaceKHR WindowManager::get_surface()
    {
        return surface;
    }
}
