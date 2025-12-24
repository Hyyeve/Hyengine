#pragma once
#include <map>

#include "hyengine/common/sized_numerics.hpp"


namespace hyengine
{
    class virtual_controller
    {
    public:
        enum class dpad
        {
            LEFT, RIGHT, UP, DOWN
        };

        void press_button(i32 button);
        void release_button(i32 button);
        void set_axis(i32 axis, f32 value);

        void press_direction(i32 pad, dpad direction);
        void release_direction(i32 pad, dpad direction);

        [[nodiscard]] bool pressed(i32 button);
        [[nodiscard]] bool released(i32 button);
        [[nodiscard]] bool pressed_this_frame(i32 button);
        [[nodiscard]] bool released_this_frame(i32 button);

        [[nodiscard]] bool direction_pressed(i32 pad, dpad direction);
        [[nodiscard]] bool direction_pressed_this_frame(i32 pad, dpad direction);

        [[nodiscard]] f32 axis_value(i32 axis);
        [[nodiscard]] bool axis_pressed(i32 axis, f32 threshold);
        [[nodiscard]] bool axis_released(i32 axis, f32 threshold);

        [[nodiscard]] i32 frame_count(i32 button);
        [[nodiscard]] i32 pressed_frames(i32 button);
        [[nodiscard]] i32 released_frames(i32 button);

        [[nodiscard]] i32 direction_frame_count(i32 pad, dpad direction);
        [[nodiscard]] i32 direction_pressed_frames(i32 pad, dpad direction);
        [[nodiscard]] i32 direction_released_frames(i32 pad, dpad direction);

        void process_inputs();

    private:
        std::map<i32, i32> button_map = std::map<i32, i32>();
        std::map<i32, f32> axis_map = std::map<i32, f32>();
        std::map<i32, std::map<dpad, i32>> pad_map = std::map<i32, std::map<dpad, i32>>();
    };
}
