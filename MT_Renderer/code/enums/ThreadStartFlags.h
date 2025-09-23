//
// Created by Sid on 9/22/2025.
//

#pragma once
#include <cstdint>

enum class ThreadStartFlags : uint32_t
{
    Running = 0,
    Detached = 1 << 0,
};
