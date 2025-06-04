#include "virtual_controller.hpp"
namespace hyengine::input
{
    void virtual_controller::press_button(const int button) {
        button_map[button] = 1;
    }

    void virtual_controller::release_button(const int button) {
        button_map[button] = -1;
    }

    void virtual_controller::set_axis(const int axis, const float value)
    {
        axis_map[axis] = value;
    }

    void virtual_controller::press_direction(const int pad, const dpad direction)
    {
        pad_map[pad][direction] = 1;
    }

    void virtual_controller::release_direction(const int pad, const dpad direction)
    {
        pad_map[pad][direction] = -1;
    }

    bool virtual_controller::pressed(const int button) {
        return button_map[button] > 0;
    }

    bool virtual_controller::released(const int button) {
        return button_map[button] < 0;
    }

    int virtual_controller::frame_count(const int button) {
        return button_map[button];
    }

    bool virtual_controller::pressed_this_frame(const int button) {
        return frame_count(button) == 1;
    }

    bool virtual_controller::released_this_frame(const int button) {
        return frame_count(button) == -1;
    }

    bool virtual_controller::direction_pressed(const int pad, const dpad direction)
    {
        return pad_map[pad][direction] > 0;
    }

    bool virtual_controller::direction_pressed_this_frame(const int pad, const dpad direction)
    {
        return pad_map[pad][direction] == 1;
    }

    float virtual_controller::axis_value(const int axis)
    {
        return axis_map[axis];
    }

    bool virtual_controller::axis_pressed(const int axis, const float threshold)
    {
        return axis_map[axis] > threshold;
    }

    bool virtual_controller::axis_released(const int axis, const float threshold)
    {
        return axis_map[axis] < threshold;
    }

    int virtual_controller::pressed_frames(const int button) {
        return std::max(0, frame_count(button));
    }

    int virtual_controller::released_frames(const int button) {
        return std::max(0, -frame_count(button));
    }

    int virtual_controller::direction_frame_count(const int pad, const dpad direction)
    {
        return pad_map[pad][direction];
    }

    int virtual_controller::direction_pressed_frames(const int pad, const dpad direction)
    {
        return std::max(direction_frame_count(pad, direction), 0);
    }

    int virtual_controller::direction_released_frames(const int pad, const dpad direction)
    {
        return std::max(0, -direction_frame_count(pad, direction));
    }

    void virtual_controller::process_inputs()
    {
        for(auto& [key, value] : button_map) {
            if(value > 0) value++;
            else if(value < 0) value--;
        }

        for(auto& [key, direction_map] : pad_map) {
            for(auto& [key, value] : direction_map) {
                if(value > 0) value++;
                else if(value < 0) value--;
            }
        }
    }
}
