//
// Created by Sid on 9/23/2025.
//

#pragma once
#include "structs/RenderContext.h"

namespace core
{
    class Application
    {
    public:
        RenderContext* render_context;
        void application_setup();
        void application_update();
    };
} // core