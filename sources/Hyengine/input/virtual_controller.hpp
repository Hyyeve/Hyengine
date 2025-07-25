#pragma once
#include <map>


namespace hyengine
{
    class virtual_controller
    {
    public:
        enum class dpad
        {
            LEFT, RIGHT, UP, DOWN
        };

        void press_button(int button);
        void release_button(int button);
        void set_axis(int axis, float value);

        void press_direction(int pad, dpad direction);
        void release_direction(int pad, dpad direction);

        [[nodiscard]] bool pressed(int button);
        [[nodiscard]] bool released(int button);
        [[nodiscard]] bool pressed_this_frame(int button);
        [[nodiscard]] bool released_this_frame(int button);

        [[nodiscard]] bool direction_pressed(int pad, dpad direction);
        [[nodiscard]] bool direction_pressed_this_frame(int pad, dpad direction);

        [[nodiscard]] float axis_value(int axis);
        [[nodiscard]] bool axis_pressed(int axis, float threshold);
        [[nodiscard]] bool axis_released(int axis, float threshold);

        [[nodiscard]] int frame_count(int button);
        [[nodiscard]] int pressed_frames(int button);
        [[nodiscard]] int released_frames(int button);

        [[nodiscard]] int direction_frame_count(int pad, dpad direction);
        [[nodiscard]] int direction_pressed_frames(int pad, dpad direction);
        [[nodiscard]] int direction_released_frames(int pad, dpad direction);

        void process_inputs();

    private:
        std::map<int, int> button_map = std::map<int, int>();
        std::map<int, float> axis_map = std::map<int, float>();
        std::map<int, std::map<dpad, int>> pad_map = std::map<int, std::map<dpad, int>>();
    };
}
