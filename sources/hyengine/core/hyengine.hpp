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
            ///Time since the application started.
            f64 time;

            ///Time since the last frame or update, depending on context.
            f64 delta_time;

            ///Interpolation value that represents how far between the previous update and the current update this frame is. Should be used alongside interpolable<x> for smooth rendering interpolation.
            f64 interpolation;

            ///Frame loop configuration information and special flags that can be updated while running. Only available during the update callback - always null during render.
            config* config;
        };

        struct config
        {
            ///Target updates per second
            u32 target_ups = 100;

            ///Target frames per second. If vsync is enabled on the window, this acts as a max framerate.
            u32 target_fps = 240;

            ///Max time an update can take, in seconds. If updates take longer than this, the game will 'slow down' to maintain accurate calculations.
            f64 max_update_time = 0.05;

            ///Max updates that can happen in a single frame.
            ///If the engine is behind on updates (an update or render took a long time), it will run at most this many updates per frame to try and catch back up.
            u32 max_updates_per_frame = 5;

            ///Whether the rendering callback should be called
            bool should_render = true;

            ///Whether the frame loop should exit. This flag must be set for the frame loop to finish running.
            bool should_exit = false;

            std::string name = "Frame Loop";

            ///Called once when the frame loop begins.
            std::function<void()> launch = [] {};

            std::function<void(loop_data)> update = [](loop_data data) {data.config->should_exit = true;};
            std::function<void(loop_data)> render = [](loop_data) {};

            ///Called once when the frame loop is exiting.
            std::function<void()> exit = [] {};
        };
    }

    ///Time since the application started.
    f64 time();

    void seed_random(u64 seed);
    u64 random();

    ///Start running a frame loop. Will not return until the loop update function writes the exit flag.
    void run_frame_loop(const frame_loop::config& config);
}
