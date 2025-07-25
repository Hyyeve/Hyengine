#pragma once
#include <functional>
#include <string>

#include "../graphics/native_window.hpp"

namespace hyengine
{
    native_window* initialize_graphics(const window_config& main_window_config);

    namespace frame_loop
    {
        struct loop_data
        {
            double time;
            double delta_time;
            double interpolation;
        };

        struct config
        {
            unsigned int target_ups = 100;
            unsigned int target_fps = 240;

            double max_frame_time = 0.1;

            std::string name = "Frame Loop";

            std::function<void()> launch = []
            {
            };

            //Time, delta
            std::function<bool(loop_data)> update = [](loop_data)
            {
                return false;
            };

            //Time, delta, interpolation
            std::function<void(loop_data)> render = [](loop_data)
            {
            };
            std::function<void()> exit = []
            {
            };
        };
    }

    double time();

    void run_frame_loop(const frame_loop::config& config);
}
