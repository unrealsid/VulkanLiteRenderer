#pragma once
#include <glm/vec4.hpp>

struct ClearState
{
    glm::vec4 color;
    float depth;
    uint32_t stencil;
};

struct RasterStateCreationParams
{
    uint32_t front_ccw = 1;
    uint32_t depth_bias = 1;
    float depth_bias_clamp = 0.0f;
    float sloped_scale_depth_bias = 0.0f;
    uint32_t depth_clip_enable = 1;
    uint32_t scissor_enable = 0;
    uint32_t multisample = 0;
    uint32_t aa_lines = 0;
};