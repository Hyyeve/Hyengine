#pragma once
#include "common.hpp"

#include <algorithm>
#include <bit>
#include <tracy/Tracy.hpp>

namespace hyengine
{
    void string_replace(std::string& str, const char from, const char to)
    {
        for (char& idx : str)
        {
            if (idx == from) idx = to;
        }
    }

    std::string stringify_utf32(const unsigned int codepoint)
    {
        std::string result;
        if (codepoint < 0x80)
        {
            result += static_cast<char>(codepoint);
        }
        else if (codepoint < 0x800)
        {
            result += static_cast<char>((codepoint >> 6) | 0xC0);
            result += static_cast<char>((codepoint & 0x3F) | 0x80);
        }
        else if (codepoint < 0x10000)
        {
            result += static_cast<char>((codepoint >> 12) | 0xE0);
            result += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
            result += static_cast<char>((codepoint & 0x3F) | 0x80);
        }
        else if (codepoint < 0x110000)
        {
            result += static_cast<char>((codepoint >> 18) | 0xF0);
            result += static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80);
            result += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
            result += static_cast<char>((codepoint & 0x3F) | 0x80);
        }

        return result;
    }

    std::string ascii_to_lower(const std::string_view& str)
    {
        ZoneScoped;
        std::string result = std::string(str);
        std::ranges::transform(result, result.begin(),[](const u8 character){ return std::tolower(character); });
        return result;
    }

    std::string ascii_to_upper(const std::string_view& str)
    {
        ZoneScoped;
        std::string result = std::string(str);
        std::ranges::transform(result, result.begin(),[](const u8 character){ return std::toupper(character); });
        return result;
    }

    template<typename value_type, typename parse_func>
    value_type parse_integer(const std::string_view& str, const value_type default_val, const int radix, const parse_func func) noexcept
    {
        ZoneScoped;
        char8* failure_check = nullptr;
        const value_type result = func(str.data(), &failure_check, radix);
        if (*failure_check) return default_val;
        return result;
    }

    u32 parse_u32(const std::string_view& str, const u32 default_val, const int radix) noexcept
    {
        return parse_integer<u32>(str, default_val, radix, std::strtoul);
    }

    u64 parse_u64(const std::string_view& str, const u64 default_val, const int radix) noexcept
    {
        return parse_integer<u64>(str, default_val, radix, std::strtoull);
    }

    i32 parse_i32(const std::string_view& str, const i32 default_val, const int radix) noexcept
    {
        return parse_integer<i32>(str, default_val, radix, std::strtol);
    }

    i64 parse_i64(const std::string_view& str, const i64 default_val, const int radix) noexcept
    {
        return parse_integer<i64>(str, default_val, radix, std::strtoll);
    }

    f32 parse_f32(const std::string_view& str, const f32 default_val) noexcept
    {
        ZoneScoped;
        char8* failure_check = nullptr;
        const f32 result = std::strtof(str.data(), &failure_check);
        if (*failure_check) return default_val;
        return result;
    }

    f64 parse_f64(const std::string_view& str, const f64 default_val) noexcept
    {
        ZoneScoped;
        char8* failure_check = nullptr;
        const f64 result = std::strtod(str.data(), &failure_check);
        if (*failure_check) return default_val;
        return result;
    }

    bool parse_bool(const std::string_view& str, const bool default_val) noexcept
    {
        const std::string tmp = ascii_to_lower(str);

        switch (string_hash(tmp))
        {
            case string_hash("true"): return true;
            case string_hash("false"): return false;
            case string_hash("yes"): return true;
            case string_hash("no"): return false;
            case string_hash("y"): return true;
            case string_hash("n"): return false;
            case string_hash("1"): return true;
            case string_hash("0"): return false;
            default: return default_val;
        }
    }

    u8 bits_required_for_value(const u64 value)
    {
        return 64 - std::countl_zero(value);
    }
}
