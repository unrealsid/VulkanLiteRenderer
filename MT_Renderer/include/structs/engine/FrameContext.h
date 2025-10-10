#pragma once

#include <memory>
#include <readerwritercircularbuffer.h>
#include "Config.inl"
#include "enums/RenderCommands.h"
#include "core/Types.h"
#include "RenderCommand.h"

/*
 * Stores frame data shared between the render and application thread.
 */

using moodycamel::BlockingReaderWriterCircularBuffer;

struct FrameContext
{
    FrameContext()= default;

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;

    BlockingReaderWriterCircularBuffer<RenderCommand> render_command_buffer{max_commands};
    BlockingReaderWriterCircularBuffer<RenderCommand> release_command_buffer{1024};

    std::vector<uint32_t> free_slots;
    std::atomic<int32_t> wait;
};
