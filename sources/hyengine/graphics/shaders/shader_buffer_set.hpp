#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "glad/glad.h"
#include "hyengine/common/id_generator.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    class shader_buffer_set
    {
    public:
        u32 allocate_slot(const GLuint buffer_id, const std::string_view attachment_name);
        void free_slot(const std::string_view attachment_name);

        void attach_to_shader(GLuint shader);

    private:
        struct buffer_slot_info
        {
            GLuint buffer_id;
            u32 slot;
        };

        id_generator<u32> slot_allocator;
        std::unordered_map<std::string, buffer_slot_info> buffers;
    };
}
