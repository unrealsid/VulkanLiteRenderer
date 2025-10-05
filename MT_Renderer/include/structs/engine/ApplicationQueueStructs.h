#pragma once
#include <glm/vec4.hpp>

struct ClearState
{
    glm::vec4 color;
    float depth;
    uint32_t stencil;
};
