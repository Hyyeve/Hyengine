#pragma once
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    u32 set_global_hotkey(const u32 key, const u32 mods);
    void clear_global_hotkey(const u32 id);
    void process_native_input();
    bool global_hotkey_pressed_this_frame(const u32 id);
}
