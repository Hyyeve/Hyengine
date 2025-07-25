#pragma once
#include <string>

namespace hyengine
{
    void portable_string_replace(std::string& str, const char from, const char to);
    std::string stringify_utf32(unsigned int codepoint);
}
