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

        update_window_dimensions();
        setup_window_resize_callback();

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

    void WindowManager::update_window_dimensions()
    {
        if (window != nullptr)
        {
            int width = 0, height = 0;
            glfwGetWindowSize(window, &width, &height);
            m_window_width = static_cast<uint32_t>(width);
            m_window_height = static_cast<uint32_t>(height);
        }
    }

    void WindowManager::setup_window_resize_callback()
    {
        if (window != nullptr)
        {
            glfwSetWindowUserPointer(window, this);
            glfwSetWindowSizeCallback(window, glfw_window_resize_callback);
        }
    }

    void WindowManager::glfw_window_resize_callback(GLFWwindow* glfw_window, int width, int height)
    {
        auto* window_manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(glfw_window));
        if (window_manager != nullptr)
        {
            window_manager->update_window_dimensions();
        }
    }
}
