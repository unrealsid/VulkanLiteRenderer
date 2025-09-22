//
// Created by Sid on 9/22/2025.
//

#pragma once

#include "structs/WindowCreationParams.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace platform
{
   class OS_Manager
    {
    public:
        explicit OS_Manager()
        {
            window = nullptr;
        }

        void init_window(WindowCreationParams const& window_creation_params);
        void load_opengl();
        void swap_buffers();
        void poll_events();
        void destroy_window();

        [[nodiscard]] GLFWwindow* get_window() const {return window;}

    private:
        GLFWwindow* window;
    };
}