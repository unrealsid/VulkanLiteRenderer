//
// Created by Sid on 9/22/2025.
//

#pragma once

#include "RenderCommand.h"
#include "core/Types.h"
#include <memory>

#include "../core/rendering/render_utils/SlotResources.hpp"
#include "../core/rendering/render_utils/RenderBuffer.hpp"

using core::render_utils::RenderBuffer;
using core::render_utils::SlotResources;

struct RenderContext
{
    explicit RenderContext(uint32_t max_commands)
           : render_command_buffer(max_commands),
             release_command_buffer(1024),
             slot_resources(2048){}

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;

    SlotResources slot_resources;

    RenderBuffer<RenderCommand> render_command_buffer;
    RenderBuffer<RenderCommand> release_command_buffer;

    std::vector<uint32_t> free_slots;
    std::atomic<int32_t> wait;
};
