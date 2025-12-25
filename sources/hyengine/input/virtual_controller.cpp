#include "virtual_controller.hpp"

#include <ranges>
#include <tracy/Tracy.hpp>

namespace hyengine
{
    void virtual_controller::press_button(const i32 button)
    {
        button_map[button] = 1;
    }

    void virtual_controller::release_button(const i32 button)
    {
        button_map[button] = -1;
    }

    void virtual_controller::set_axis(const i32 axis, const f32 value)
    {
        axis_map[axis] = value;
    }

    void virtual_controller::press_direction(const i32 pad, const dpad direction)
    {
        pad_map[pad][direction] = 1;
    }

    void virtual_controller::release_direction(const i32 pad, const dpad direction)
    {
        pad_map[pad][direction] = -1;
    }

    bool virtual_controller::pressed(const i32 button)
    {
        return button_map[button] > 0;
    }

    bool virtual_controller::released(const i32 button)
    {
        return button_map[button] < 0;
    }

    bool virtual_controller::pressed_this_frame(const i32 button)
    {
        return frame_count(button) == 1;
    }

    bool virtual_controller::released_this_frame(const i32 button)
    {
        return frame_count(button) == -1;
    }

    bool virtual_controller::direction_pressed(const i32 pad, const dpad direction)
    {
        return pad_map[pad][direction] > 0;
    }

    bool virtual_controller::direction_pressed_this_frame(const i32 pad, const dpad direction)
    {
        return pad_map[pad][direction] == 1;
    }

    f32 virtual_controller::axis_value(const i32 axis)
    {
        return axis_map[axis];
    }

    bool virtual_controller::axis_pressed(const i32 axis, const f32 threshold)
    {
        return axis_map[axis] > threshold;
    }

    bool virtual_controller::axis_released(const i32 axis, const f32 threshold)
    {
        return axis_map[axis] < threshold;
    }

    i32 virtual_controller::frame_count(const i32 button)
    {
        return button_map[button];
    }

    i32 virtual_controller::pressed_frames(const i32 button)
    {
        return std::max(0, frame_count(button));
    }

    i32 virtual_controller::released_frames(const i32 button)
    {
        return std::max(0, -frame_count(button));
    }

    i32 virtual_controller::direction_frame_count(const i32 pad, const dpad direction)
    {
        return pad_map[pad][direction];
    }

    i32 virtual_controller::direction_pressed_frames(const i32 pad, const dpad direction)
    {
        return std::max(direction_frame_count(pad, direction), 0);
    }

    i32 virtual_controller::direction_released_frames(const i32 pad, const dpad direction)
    {
        return std::max(0, -direction_frame_count(pad, direction));
    }

    void virtual_controller::process_inputs()
    {
        ZoneScoped;
        for (i32& value : button_map | std::views::values)
        {
            if (value > 0) value++;
            else if (value < 0) value--;
        }

        for (std::map<dpad, int>& direction_map : pad_map | std::views::values)
        {
            for (i32& value : direction_map | std::views::values)
            {
                if (value > 0) value++;
                else if (value < 0) value--;
            }
        }
    }
}
