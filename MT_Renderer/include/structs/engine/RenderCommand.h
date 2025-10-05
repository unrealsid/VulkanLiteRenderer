#pragma once
#include <cstdint>
#include <variant>

#include "ApplicationQueueStructs.h"
#include "Commands.h"

using namespace commands;

struct RenderCommand
{
    //What does this command do?
    uint32_t command_index{};

    //What resource ID should it use on the render thread?
    uint32_t resource_slot{};

    //What frame?
    uint64_t frame_index{};

    //Additional data
    std::variant<ClearState> render_data;
};
