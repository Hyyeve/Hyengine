#pragma once
#include <cstdint>

#include "glad/glad.h"
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

    class gl_object_state
    {
    public:
        virtual ~gl_object_state() = default;
        [[nodiscard]] virtual descriptor_type object_type() const = 0;
    };

    struct gl_signal_state
    {
        GLsync sync_point;
    };
}
