#pragma once
#include <string>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    void string_replace(std::string& str, const char from, const char to);
    std::string stringify_utf32(u32 codepoint);

    std::string ascii_to_lower(const std::string_view& str);
    std::string ascii_to_upper(const std::string_view& str);

    constexpr u32 string_hash(const std::string_view& string) noexcept
    {
        u32 hash_value = 5381;
        for (const u8 c : string) hash_value = (hash_value << 5u) + hash_value + static_cast<unsigned char>(c);
        return hash_value;
    }

    u32 parse_u32(const std::string_view& str, const u32 default_val, const int radix = 0) noexcept;
    u64 parse_u64(const std::string_view& str, const u64 default_val, const int radix = 0) noexcept;
    i32 parse_i32(const std::string_view& str, const i32 default_val, const int radix = 0) noexcept;
    i64 parse_i64(const std::string_view& str, const i64 default_val, const int radix = 0) noexcept;
    f32 parse_f32(const std::string_view& str, const f32 default_val) noexcept;
    f64 parse_f64(const std::string_view& str, const f64 default_val) noexcept;
    bool parse_bool(const std::string_view& str, const bool default_val) noexcept;
}
