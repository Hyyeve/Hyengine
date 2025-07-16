#include "virtual_mouse.hpp"

#include <tracy/Tracy.hpp>


namespace hyengine::input
{
    using namespace glm;

    void virtual_mouse::move_relative(const vec2 delta)
    {
        ZoneScoped;
        mouse_delta += delta;
        mouse_position += delta;
    }

    void virtual_mouse::move_absolute(const vec2 position)
    {
        ZoneScoped;
        mouse_delta += position - mouse_position;
        mouse_position = position;
    }

    void virtual_mouse::scroll(const vec2 delta)
    {
        ZoneScoped;
        scroll_delta += delta;
        total_scroll += delta;
    }

    void virtual_mouse::click_button(const int button)
    {
        ZoneScoped;
        button_map[button] = 1;
    }

    void virtual_mouse::release_button(const int button)
    {
        ZoneScoped;
        button_map[button] = -1;
    }

    bool virtual_mouse::clicked(const int button)
    {
        ZoneScoped;
        return button_map[button] > 0;
    }

    bool virtual_mouse::released(const int button)
    {
        ZoneScoped;
        return button_map[button] < 0;
    }

    bool virtual_mouse::clicked_this_frame(const int button)
    {
        ZoneScoped;
        return button_map[button] == 1;
    }

    bool virtual_mouse::released_this_frame(const int button)
    {
        ZoneScoped;
        return button_map[button] == -1;
    }

    int virtual_mouse::frame_count(const int button)
    {
        ZoneScoped;
        return button_map[button];
    }

    int virtual_mouse::pressed_frames(const int button)
    {
        ZoneScoped;
        return button_map[button] > 0 ? button_map[button] : 0;
    }

    int virtual_mouse::released_frames(const int button)
    {
        ZoneScoped;
        return button_map[button] < 0 ? -button_map[button] : 0;
    }

    vec2 virtual_mouse::get_position() const
    {
        ZoneScoped;
        return mouse_position;
    }

    vec2 virtual_mouse::get_position_delta() const
    {
        ZoneScoped;
        return mouse_delta;
    }

    vec2 virtual_mouse::get_total_scroll() const
    {
        ZoneScoped;
        return total_scroll;
    }

    vec2 virtual_mouse::get_scroll_delta() const
    {
        ZoneScoped;
        return scroll_delta;
    }

    void virtual_mouse::process_inputs()
    {
        ZoneScoped;
        mouse_delta = vec2();
        scroll_delta = vec2();
        for (auto& [button, value] : button_map)
        {
            if (value > 0) value++;
            else if (value < 0) value--;
        }
    }
}
