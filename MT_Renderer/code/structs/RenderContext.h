//
// Created by Sid on 9/22/2025.
//

#pragma once

#include "RenderCommand.h"
#include "core/Types.h"
#include <memory>
#include "core/rendering/RenderData.hpp"

struct RenderContext
{
    RenderContext(uint32_t max_commands)
           : render_command_buffer(max_commands),
             release_command_buffer(1024) {}

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;
    RingBuffer<RenderCommand> render_command_buffer;
    RingBuffer<RenderCommand> release_command_buffer;
};
