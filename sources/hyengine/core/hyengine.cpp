#include <functional>

#include "hyengine.hpp"

#include <iostream>
#include <tracy/Tracy.hpp>

#include "logger.hpp"
#include "../input/input.hpp"


namespace hyengine
{
    using namespace hyengine;


    native_window* initialize_graphics(const window_config& main_window_config)
    {
        ZoneScopedC(0x0077FF);
        if (!try_init_glfw())
        {
            log_error(logger_tags::ENGINE, "Failed to initialize glfw");
            return nullptr;
        }

        native_window* window = new native_window(main_window_config);

        if (!window->exists())
        {
            log_error(logger_tags::ENGINE, "Failed to create main window");
            delete window;
            return nullptr;
        }

        window->apply_context();

        bind_input(window->get_handle());

        if (!try_load_glad())
        {
            log_error(logger_tags::ENGINE, "Failed to load glad");
            delete window;
            return nullptr;
        }

        if (main_window_config.gl_profile_debug) enable_gl_debug();

        set_viewport(window->get_viewport());

        return window;
    }

    f64 time()
    {
        return glfwGetTime();
    }

    void launch_frame_loop(const frame_loop::config& config)
    {
        log_info(logger_tags::ENGINE, "Launching frame loop '", config.name, "'");
        config.launch();
    }

    void spin_frame_loop(frame_loop::config config)
    {
        log_info(logger_tags::ENGINE, "Entering frame loop");

        frame_loop::loop_data loop_data {};

        f64 current_time = time();
        f64 runtime = 0.0f;
        f64 update_accumulator = 0.0f;
        f64 frame_accumulator = 0.0f;
        loop_data.config = &config;

        while (true)
        {
            const bool should_update = config.should_update;
            const bool should_render = config.should_render;
            const f64 update_step_time = 1.0 / config.target_ups;
            const f64 max_frame_time = config.max_frame_time;
            const f64 min_frame_time = 1.0 / config.target_fps;
            u32 update_budget = config.max_updates_per_frame;

            const f64 new_time = time();
            f64 delta_time = new_time - current_time;
            current_time = new_time;

            loop_data.delta_time = update_step_time;

            if (delta_time > max_frame_time)
            {
                log_warn(logger_tags::ENGINE, "Last frame took too long! ", stringify_secs(delta_time), ", but max allowed is ", stringify_secs(max_frame_time));
                delta_time = max_frame_time;
            }

            if (should_update) update_accumulator += delta_time;
            while (update_accumulator >= update_step_time && update_budget > 0 && should_update)
            {
                ZoneScopedNC("Update", 0xFF0077);
                FrameMarkStart("Update");
                config.update(loop_data);
                FrameMarkEnd("Update");

                if (config.should_exit) return;

                update_budget--;
                runtime += update_step_time;
                update_accumulator -= update_step_time;
            }

            if (should_render) frame_accumulator += delta_time;
            if (frame_accumulator >= min_frame_time && should_render)
            {
                FrameMarkStart("Render");
                ZoneScopedNC("Render", 0x0077FF);
                const f64 interpolation_delta = glm::fract(update_accumulator) / update_step_time;
                const f64 interpolated_runtime = runtime + update_step_time * interpolation_delta;
                config.render({interpolated_runtime, frame_accumulator, interpolation_delta});
                frame_accumulator = 0;
                FrameMarkEnd("Render");
            }

            FrameMarkNamed("Hyengine Game Loop");
        }
    }

    void exit_frame_loop(const frame_loop::config& config)
    {
        log_info(logger_tags::ENGINE, "Exiting frame loop");
        config.exit();
    }

    void run_frame_loop(const frame_loop::config& config)
    {
        ZoneScopedNC("Hyengine Game Loop", 0x7700FF);

        const f64 start_time = time();

        launch_frame_loop(config);
        spin_frame_loop(config);
        exit_frame_loop(config);

        log_info(logger_tags::ENGINE, "Ran for: ", stringify_secs(time() - start_time));
    }
}
