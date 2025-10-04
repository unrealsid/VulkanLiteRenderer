#pragma once

#include <memory>
#include <readerwritercircularbuffer.h>
#include "Config.inl"
#include "enums/RenderCommands.h"
#include "core/Types.h"
#include "RenderCommand.h"
#include "render/render_utils/RenderBuffer.hpp"
#include "render/render_utils/SlotResources.hpp"

/*
 * Stores frame data that is shared between the render and application thread.
 */

using core::render_utils::RenderBuffer;
using core::render_utils::SlotResources;
using moodycamel::BlockingReaderWriterCircularBuffer;

struct FrameContext
{
    explicit FrameContext(uint32_t max_commands)
           : slot_resources(2048){}

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;

    SlotResources slot_resources;

    BlockingReaderWriterCircularBuffer<RenderCommand> render_command_buffer{max_commands};
    BlockingReaderWriterCircularBuffer<RenderCommand> release_command_buffer{1024};

    std::vector<uint32_t> free_slots;
    std::atomic<int32_t> wait;
};
