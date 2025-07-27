#include <functional>

#include "hyengine.hpp"

#include <tracy/Tracy.hpp>

#include "logger.hpp"
#include "../input/input.hpp"


namespace hyengine
{
    using namespace hyengine;

    const std::string logger_tag = "Hyengine";

    f64 time()
    {
        return glfwGetTime();
    }

    native_window* initialize_graphics(const window_config& main_window_config)
    {
        ZoneScopedC(0x0077FF);
        if (!try_init_glfw())
        {
            log_error(logger_tag, "Failed to initialize glfw");
            return nullptr;
        }

        native_window* window = new native_window(main_window_config);

        if (!window->exists())
        {
            log_error(logger_tag, "Failed to create main window");
            delete window;
            return nullptr;
        }

        window->apply_context();

        bind_input(window->get_handle());

        if (!try_load_glad())
        {
            log_error(logger_tag, "Failed to load glad");
            delete window;
            return nullptr;
        }

        if (main_window_config.gl_profile_debug) enable_gl_debug();

        set_viewport(window->get_viewport());

        return window;
    }

    void run_frame_loop(const frame_loop::config& config)
    {
        ZoneScopedNC("Hyengine Game Loop", 0x7700FF);

        log_info(config.name, "Launching frame loop");

        config.launch();

        const f64 update_step_time = 1.0 / config.target_ups;
        const f64 max_frame_time = config.max_frame_time;
        const f64 min_frame_time = 1.0 / config.target_fps;

        f64 runtime = 0.0;
        f64 current_time = time();
        f64 accumulator = 0.0;

        const f64 start_time = current_time;

        unsigned long update_count = 0;
        unsigned long frame_count = 0;
        u32 updates_last_frame = 0;

        log_info(config.name, "Starting updates / rendering");

        while (true)
        {
            const f64 new_time = time();
            f64 frame_time = new_time - current_time;
            const f64 ups_last_frame = updates_last_frame / frame_time;

            if (updates_last_frame > 0 && ups_last_frame < config.target_ups)
            {
                log_warn(config.name, "Last update step took longer than expected! (", stringify_secs(frame_time), " for ", stringify_count(updates_last_frame, "update"), ")");
            }

            updates_last_frame = 0;

            if (frame_time > max_frame_time)
            {
                log_warn(config.name, "Last frame took too long! ", stringify_secs(frame_time), ", but max allowed is ", stringify_secs(max_frame_time));
                frame_time = max_frame_time;
            }

            if (frame_time < min_frame_time) continue;

            current_time = new_time;
            accumulator += frame_time;

            bool should_continue = true;

            while (accumulator >= update_step_time)
            {
                ZoneScopedNC("Update", 0xFF0077);
                FrameMarkStart("Update");
                should_continue = config.update({runtime, update_step_time, 0});
                FrameMarkEnd("Update");

                if (!should_continue) break;

                runtime += update_step_time;
                accumulator -= update_step_time;
                update_count++;
                updates_last_frame++;
            }

            if (!should_continue) break;


            {
                FrameMarkStart("Render");
                ZoneScopedNC("Render", 0x0077FF);
                const f64 i32erpolation_delta = accumulator / update_step_time;
                config.render({runtime + frame_time * i32erpolation_delta, frame_time, i32erpolation_delta});
                frame_count++;
                FrameMarkEnd("Render");
            }

            FrameMarkNamed("Hyengine Game Loop");
        }

        log_info(config.name, "Exiting frame loop");
        const f64 total_runtime = current_time - start_time;

        log_info(config.name, "Total runtime: ", stringify_secs(total_runtime));
    }
}
