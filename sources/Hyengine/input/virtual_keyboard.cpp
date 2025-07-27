#include "virtual_keyboard.hpp"

#include <ranges>
#include <tracy/Tracy.hpp>

#include "../core/portability.hpp"
#include "../library/gl.hpp"

namespace hyengine
{
    void virtual_keyboard::type_character(const u32 codepoint)
    {
        current_text += stringify_utf32(codepoint);
    }

    void virtual_keyboard::press_key(const i32 key)
    {
        key_map[key] = 1;
    }

    void virtual_keyboard::release_key(const i32 key)
    {
        key_map[key] = -1;
    }

    bool virtual_keyboard::pressed(const i32 key)
    {
        return key_map[key] > 0;
    }

    bool virtual_keyboard::released(const i32 key)
    {
        return key_map[key] < 0;
    }

    i32 virtual_keyboard::frame_count(const i32 key)
    {
        return key_map[key];
    }

    bool virtual_keyboard::pressed_this_frame(const i32 key)
    {
        return frame_count(key) == 1;
    }

    bool virtual_keyboard::released_this_frame(const i32 key)
    {
        return frame_count(key) == -1;
    }

    i32 virtual_keyboard::pressed_frames(const i32 key)
    {
        return std::max(0, frame_count(key));
    }

    i32 virtual_keyboard::released_frames(const i32 key)
    {
        return std::max(0, -frame_count(key));
    }

    bool virtual_keyboard::control_pressed()
    {
        return pressed(GLFW_KEY_LEFT_CONTROL) || pressed(GLFW_KEY_RIGHT_CONTROL);
    }

    bool virtual_keyboard::shift_pressed()
    {
        return pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    }

    bool virtual_keyboard::alt_pressed()
    {
        return pressed(GLFW_KEY_LEFT_ALT) || pressed(GLFW_KEY_RIGHT_ALT);
    }

    bool virtual_keyboard::super_pressed()
    {
        return pressed(GLFW_KEY_LEFT_SUPER) || pressed(GLFW_KEY_RIGHT_SUPER);
    }

    std::string virtual_keyboard::typed_text() const
    {
        return current_text;
    }

    void virtual_keyboard::process_inputs()
    {
        ZoneScoped;
        for (i32& value : key_map | std::views::values)
        {
            if (value > 0) value++;
            else if (value < 0) value--;
        }

        current_text.clear();
    }
}
