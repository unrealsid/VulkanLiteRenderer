#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace commands
{
    struct ClearState
    {
        float r, g, b, a;
        float depth;
        GLbitfield clear;
    };

    struct RasterStateCreateParams
    {
        GLenum fill_mode = GL_FILL;
        GLenum cull_mode = GL_BACK;
        GLenum front_ccw = GL_CCW;
        GLfloat depth_bias = 0.0f;
        bool depth_clip_enable = true;
    };

    struct Draw
    {
        uint32_t vertex_count;
        uint32_t start_vertex;

        GLuint buffer_id;
        GLuint vertex_buffer;
        GLuint index_buffer;
    };
}
