#pragma once
#include <string>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    void portable_string_replace(std::string& str, const char from, const char to);
    std::string stringify_utf32(u32 codepoint);
}
