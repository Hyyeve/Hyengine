#include "portability.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    void string_replace(std::string& str, const char from, const char to)
    {
        ZoneScoped;
        for (char& idx : str)
        {
            if (idx == from) idx = to;
        }
    }

    std::string utf32_stringify(const unsigned int codepoint)
    {
        ZoneScoped;
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
}
