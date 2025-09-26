#pragma once
#include <cstdint>

enum class ResourceFlags : uint32_t
{
    Free = 1,
    Used = 1 << 1
};
