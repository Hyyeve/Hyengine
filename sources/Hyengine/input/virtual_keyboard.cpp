#include "virtual_keyboard.hpp"

#include <ranges>
#include <tracy/Tracy.hpp>

#include "../common/portability.hpp"
#include "../library/gl.hpp"

namespace hyengine
{
    void virtual_keyboard::type_character(const unsigned int codepoint)
    {
        ZoneScoped;
        current_text += utf32_stringify(codepoint);
    }

    void virtual_keyboard::press_key(const int key)
    {
        ZoneScoped;
        key_map[key] = 1;
    }

    void virtual_keyboard::release_key(const int key)
    {
        ZoneScoped;
        key_map[key] = -1;
    }

    bool virtual_keyboard::pressed(const int key)
    {
        ZoneScoped;
        return key_map[key] > 0;
    }

    bool virtual_keyboard::released(const int key)
    {
        ZoneScoped;
        return key_map[key] < 0;
    }

    int virtual_keyboard::frame_count(const int key)
    {
        ZoneScoped;
        return key_map[key];
    }

    bool virtual_keyboard::pressed_this_frame(const int key)
    {
        ZoneScoped;
        return frame_count(key) == 1;
    }

    bool virtual_keyboard::released_this_frame(const int key)
    {
        ZoneScoped;
        return frame_count(key) == -1;
    }

    int virtual_keyboard::pressed_frames(const int key)
    {
        ZoneScoped;
        return std::max(0, frame_count(key));
    }

    int virtual_keyboard::released_frames(const int key)
    {
        ZoneScoped;
        return std::max(0, -frame_count(key));
    }

    bool virtual_keyboard::control_pressed()
    {
        ZoneScoped;
        return pressed(GLFW_KEY_LEFT_CONTROL) || pressed(GLFW_KEY_RIGHT_CONTROL);
    }

    bool virtual_keyboard::shift_pressed()
    {
        ZoneScoped;
        return pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    }

    bool virtual_keyboard::alt_pressed()
    {
        ZoneScoped;
        return pressed(GLFW_KEY_LEFT_ALT) || pressed(GLFW_KEY_RIGHT_ALT);
    }

    bool virtual_keyboard::super_pressed()
    {
        ZoneScoped;
        return pressed(GLFW_KEY_LEFT_SUPER) || pressed(GLFW_KEY_RIGHT_SUPER);
    }

    std::string virtual_keyboard::typed_text() const
    {
        ZoneScoped;
        return current_text;
    }

    void virtual_keyboard::process_inputs()
    {
        ZoneScoped;
        for (int& value : key_map | std::views::values)
        {
            if (value > 0) value++;
            else if (value < 0) value--;
        }

        current_text.clear();
    }
}
