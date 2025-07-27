#pragma once

#include <map>
#include <string>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    class virtual_keyboard
    {
    public:
        void type_character(u32 codepoint);
        void press_key(i32 key);
        void release_key(i32 key);

        [[nodiscard]] bool pressed(i32 key);
        [[nodiscard]] bool released(i32 key);
        [[nodiscard]] bool pressed_this_frame(i32 key);
        [[nodiscard]] bool released_this_frame(i32 key);

        [[nodiscard]] i32 frame_count(i32 key);
        [[nodiscard]] i32 pressed_frames(i32 key);
        [[nodiscard]] i32 released_frames(i32 key);

        [[nodiscard]] bool control_pressed();
        [[nodiscard]] bool shift_pressed();
        [[nodiscard]] bool alt_pressed();
        [[nodiscard]] bool super_pressed();

        [[nodiscard]] std::string typed_text() const;

        void process_inputs();

    private:
        std::map<i32, i32> key_map = std::map<i32, i32>();
        std::string current_text;
    };
}
