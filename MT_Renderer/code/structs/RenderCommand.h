#pragma once
#include <cstdint>
#include "Commands.h"

using namespace commands;

struct RenderCommand
{
    uint32_t command_index;
    uint32_t resource_slot;
    uint64_t frame_index;

    union
    {
        ClearState clear_state_params;
        
    };
};
