//
// Created by Sid on 9/22/2025.
//

#pragma once

#include "structs/WindowCreateParams.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace platform
{
   class OS_Manager
    {
    public:
        explicit OS_Manager() : window(nullptr), window_creation_params({})
        {
        }

        void init_window(WindowCreateParams& creation_params);
        void swap_buffers();
        void poll_events();
        void destroy_window();

       void os_update();

        [[nodiscard]] GLFWwindow* get_window() const {return window;}

    private:
        GLFWwindow* window;
        WindowCreateParams window_creation_params;
    };
}