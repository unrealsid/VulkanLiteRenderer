#pragma once

#include <memory>
#include <readerwritercircularbuffer.h>
#include "Config.inl"
#include "core/Types.h"
/*
 * Stores frame data shared between the render and application thread.
 */

using moodycamel::BlockingReaderWriterCircularBuffer;

struct FrameContext
{
    explicit FrameContext()= default;

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;

    //BlockingReaderWriterCircularBuffer<RenderCommand> render_command_buffer{max_commands};
};
