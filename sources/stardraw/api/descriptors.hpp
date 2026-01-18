#pragma once
#include "types.hpp"

namespace stardraw
{
    enum class buffer_type : uint8_t
    {
        PERSISTANT, STREAMING
    };

    struct buffer_descriptor final : descriptor
    {
        explicit buffer_descriptor(const std::string_view& name, const uint64_t size, const buffer_type type = buffer_type::PERSISTANT) : descriptor(name), size(size), buff_type(type) {}

        [[nodiscard]] descriptor_type type() const override
        {
            return descriptor_type::BUFFER;
        }

        uint64_t size;
        buffer_type buff_type;
    };
}
