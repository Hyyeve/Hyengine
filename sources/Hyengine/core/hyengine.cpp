#include <functional>

#include "hyengine.hpp"

#include <tracy/Tracy.hpp>

#include "logger.hpp"
#include "../input/input.hpp"


namespace hyengine
{
    using namespace hyengine;

    const std::string logger_tag = "Hyengine";

    double time()
    {
        ZoneScoped;
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

        const double update_step_time = 1.0 / config.target_ups;
        const double max_frame_time = config.max_frame_time;
        const double min_frame_time = 1.0 / config.target_fps;

        double runtime = 0.0;
        double current_time = time();
        double accumulator = 0.0;

        const double start_time = current_time;

        unsigned long update_count = 0;
        unsigned long frame_count = 0;
        unsigned int updates_last_frame = 0;

        log_info(config.name, "Starting updates / rendering");

        while (true)
        {
            const double new_time = time();
            double frame_time = new_time - current_time;
            const double ups_last_frame = updates_last_frame / frame_time;

            if (updates_last_frame > 0 && ups_last_frame < config.target_ups)
            {
                log_warn(config.name, "Last update step took longer than expected! (", format_secs(frame_time), " for ", format_count(updates_last_frame, "update"), ")");
            }

            updates_last_frame = 0;

            if (frame_time > max_frame_time)
            {
                log_warn(config.name, "Last frame took too long! ", format_secs(frame_time), ", but max allowed is ", format_secs(max_frame_time));
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
                const double interpolation_delta = accumulator / update_step_time;
                config.render({runtime + frame_time * interpolation_delta, frame_time, interpolation_delta});
                frame_count++;
                FrameMarkEnd("Render");
            }

            FrameMarkNamed("Hyengine Game Loop");
        }

        log_info(config.name, "Exiting frame loop");
        const double total_runtime = current_time - start_time;

        log_info(config.name, "Total runtime: ", format_secs(total_runtime));
    }
}
