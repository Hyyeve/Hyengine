#pragma once
#include "types.hpp"

namespace stardraw
{
    struct buffer_descriptor final : descriptor
    {
        explicit buffer_descriptor(const uint64_t size, const uint8_t multibuffer_count = 1, const bool require_cpu_access = false) : size(size), multibuffer_count(multibuffer_count), require_cpu_access(require_cpu_access) {}

        [[nodiscard]] descriptor_type type() override
        {
            return descriptor_type::BUFFER;
        }

        uint64_t size = 0;
        uint8_t multibuffer_count = 1;
        bool require_cpu_access = false;
    };
}
