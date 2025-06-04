#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace hyengine::input {
    class virtual_keyboard {
    public:
        void type_character(unsigned int codepoint);
        void press_key(int key);
        void release_key(int key);

        [[nodiscard]] bool pressed(int key);
        [[nodiscard]] bool released(int key);
        [[nodiscard]] bool pressed_this_frame(int key);
        [[nodiscard]] bool released_this_frame(int key);

        [[nodiscard]] int frame_count(int key);
        [[nodiscard]] int pressed_frames(int key);
        [[nodiscard]] int released_frames(int key);

        [[nodiscard]] bool control_pressed();
        [[nodiscard]] bool shift_pressed();
        [[nodiscard]] bool alt_pressed();
        [[nodiscard]] bool super_pressed();

        [[nodiscard]] std::string typed_text() const;

        void process_inputs();

    private:
        std::map<int, int> key_map = std::map<int, int>();
        std::string current_text;
    };
}


