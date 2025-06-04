#include "virtual_keyboard.hpp"

#include "../common/portability.hpp"
#include "../library/gl.hpp"

namespace hyengine::input {

    void virtual_keyboard::type_character(const unsigned int codepoint)
    {
        current_text += common::portability::utf32_stringify(codepoint);
    }

    void virtual_keyboard::press_key(const int key) {
        key_map[key] = 1;
    }

    void virtual_keyboard::release_key(const int key) {
        key_map[key] = -1;
    }

    bool virtual_keyboard::pressed(const int key) {
        return key_map[key] > 0;
    }

    bool virtual_keyboard::released(const int key) {
        return key_map[key] < 0;
    }

    int virtual_keyboard::frame_count(const int key) {
        return key_map[key];
    }

    bool virtual_keyboard::pressed_this_frame(const int key) {
        return frame_count(key) == 1;
    }

    bool virtual_keyboard::released_this_frame(const int key) {
        return frame_count(key) == -1;
    }

    int virtual_keyboard::pressed_frames(const int key) {
        return std::max(0, frame_count(key));
    }

    int virtual_keyboard::released_frames(const int key) {
        return std::max(0, -frame_count(key));
    }

    bool virtual_keyboard::control_pressed() {
        return pressed(GLFW_KEY_LEFT_CONTROL) || pressed(GLFW_KEY_RIGHT_CONTROL);
    }

    bool virtual_keyboard::shift_pressed() {
        return pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    }

    bool virtual_keyboard::alt_pressed() {
        return pressed(GLFW_KEY_LEFT_ALT) || pressed(GLFW_KEY_RIGHT_ALT);
    }

    bool virtual_keyboard::super_pressed() {
        return pressed(GLFW_KEY_LEFT_SUPER) || pressed(GLFW_KEY_RIGHT_SUPER);
    }

    std::string virtual_keyboard::typed_text() const
    {
        return current_text;
    }

    void virtual_keyboard::process_inputs() {
        for(auto& [key, value] : key_map) {
            if(value > 0) value++;
            else if(value < 0) value--;
        }

        current_text.clear();
    }
}