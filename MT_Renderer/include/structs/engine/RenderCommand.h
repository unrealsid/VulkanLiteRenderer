#pragma once
#include <cstdint>
#include <variant>

#include "Commands.h"

using namespace commands;

struct RenderCommand
{
    uint32_t command_index{};
    uint32_t resource_slot{};
    uint64_t frame_index{};
};
