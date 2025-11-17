#pragma once

#include <memory>
#include <readerwritercircularbuffer.h>
#include "Config.inl"
#include "core/Types.h"
#include "platform/WindowManager.h"
/*
 * Stores engine data shared between the render and application thread.
 */

using moodycamel::BlockingReaderWriterCircularBuffer;

struct EngineContext
{
    explicit EngineContext()= default;

    std::unique_ptr<Semaphore> consume_semaphore;
    std::unique_ptr<Semaphore> continue_semaphore;

    std::shared_ptr<platform::WindowManager> window_manager;

    //BlockingReaderWriterCircularBuffer<RenderCommand> render_command_buffer{max_commands};
    std::atomic<bool> start_rendering = false;
};
