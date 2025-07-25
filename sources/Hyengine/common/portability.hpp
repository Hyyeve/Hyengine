#pragma once
#include <string>

namespace hyengine
{
    void string_replace(std::string& str, const char from, const char to);
    std::string utf32_stringify(unsigned int codepoint);
}
