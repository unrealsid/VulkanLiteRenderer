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

    struct Draw
    {
        uint32_t vertex_count;
        uint32_t start_vertex;

        GLuint buffer_id;
        GLuint vertex_buffer;
        GLuint index_buffer;
    };
}
