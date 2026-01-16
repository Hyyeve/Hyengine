#pragma once
#include <cstdint>

#include "stardraw/api/descriptors.hpp"

namespace stardraw
{
    #pragma pack(push, 1)
    struct gl_draw_arrays_indirect_params
    {
        uint32_t vertex_count;
        uint32_t instance_count;
        uint32_t vertex_begin;
        uint32_t instance_begin;
    };

    struct gl_draw_elements_indirect_params
    {
        uint32_t vertex_count;
        uint32_t instance_count;
        uint32_t index_begin;
        int32_t vertex_begin;
        uint32_t instance_begin;
    };
    #pragma pack(pop)

    struct gl_object_state
    {
        virtual ~gl_object_state() = 0;
        descriptor* object_descriptor = nullptr;
    };
}
