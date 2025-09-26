#pragma once
#include <cstdint>

#include "enums/ResourceFlags.h"

struct FreeSlot
{
    uint32_t index = 0;
    uint32_t next = 0;
    ResourceFlags flags = ResourceFlags::Free;
};
