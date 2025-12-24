#pragma once
#include <functional>
#include <string>

#include "../graphics/native_window.hpp"

namespace hyengine
{
    native_window* initialize_graphics(const window_config& main_window_config);

    namespace frame_loop
    {
        struct config;

        struct loop_data
        {
            f64 time;
            f64 delta_time;
            f64 interpolation;
            config* config;
        };

        struct config
        {
            u32 target_ups = 100;
            u32 target_fps = 240;
            f64 max_frame_time = 0.1;
            u32 max_updates_per_frame = 5;

            bool should_render = true;
            bool should_update = true;
            bool should_exit = false;

            std::string name = "Frame Loop";

            std::function<void()> launch = [] {};

            //Time, delta
            std::function<void(loop_data)> update = [](loop_data) {};

            //Time, delta, interpolation
            std::function<void(loop_data)> render = [](loop_data) {};
            std::function<void()> exit = [] {};
        };
    }

    f64 time();

    void run_frame_loop(const frame_loop::config& config);
}
