#pragma once
namespace hyengine::input::native {
    unsigned int set_global_hotkey(const unsigned int key, const unsigned int mods);
    void clear_global_hotkey(const unsigned int id);
    void process_native_input();
    bool global_hotkey_pressed_this_frame(const unsigned int id);
}