#include "native_window.hpp"

#include <tracy/Tracy.hpp>

#include "../core/hyengine.hpp"
#include "../core/logger.hpp"

namespace hyengine
{
    native_window::native_window(const window_config& config, const native_window* parent)
    {
        ZoneScoped;

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.gl_version_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.gl_version_minor);

        glfwWindowHint(GLFW_OPENGL_PROFILE, config.gl_profile_compatibility ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, config.gl_profile_debug);

        glfwWindowHint(GLFW_DECORATED, config.decorated ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, config.visible ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, config.transparent ? GLFW_TRUE : GLFW_FALSE);

        viewport = {0, 0, width, height};
        current_monitor = glfwGetPrimaryMonitor();
        creation_time = hyengine::time();
        is_visible = config.visible;

        GLFWwindow* share = parent == nullptr ? nullptr : parent->get_handle();
        handle = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, share);

        if (handle != nullptr)
        {
            glfwGetWindowPos(handle, &x, &y);
            glfwSetWindowUserPointer(handle, this);
            glfwSetWindowSizeCallback(handle, update_size);
            glfwSetWindowPosCallback(handle, update_position);
        }

        set_icon(config.icon);
        set_size(config.width, config.height);
        set_window_mode(config.mode);
        set_vsync(config.vsync ? 1 : 0);
    }

    native_window::~native_window()
    {
        ZoneScoped;
        if (handle != nullptr) glfwDestroyWindow(handle);
    }

    void native_window::set_title(const char* title) const
    {
        ZoneScoped;
        glfwSetWindowTitle(handle, title);
    }

    void native_window::set_icon(const GLFWimage* icon) const
    {
        ZoneScoped;
        if (icon != nullptr)
        {
            glfwSetWindowIcon(handle, 1, icon);
        }
        else
        {
            glfwSetWindowIcon(handle, 0, nullptr);
        }
    }

    void native_window::set_opacity(const f32 opacity) const
    {
        ZoneScoped;
        glfwSetWindowOpacity(handle, opacity);
    }

    void native_window::set_size(const i32 width, const i32 height)
    {
        ZoneScoped;
        glfwSetWindowSize(handle, width, height);
        update_current_monitor();
    }

    void native_window::set_position(const i32 x, const i32 y)
    {
        ZoneScoped;
        glfwSetWindowPos(handle, x, y);
        update_current_monitor();
    }

    void native_window::set_decorated(const bool decorations) const
    {
        ZoneScoped;
        glfwSetWindowAttrib(handle, GLFW_DECORATED, decorations);
    }

    void native_window::center_position()
    {
        ZoneScoped;
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        i32 current_width = 0;
        i32 current_height = 0;
        glfwGetWindowSize(handle, &current_width, &current_height);
        const i32 x = (mode->width - current_width) / 2;
        const i32 y = (mode->height - current_height) / 2;
        glfwSetWindowPos(handle, x, y);
        update_current_monitor();
    }

    void native_window::set_window_mode(const window_mode mode)
    {
        ZoneScoped;
        switch (mode)
        {
            case window_mode::FULLSCREEN:
            {
                if (current_monitor == nullptr) current_monitor = glfwGetPrimaryMonitor();
                if (current_monitor == nullptr)
                {
                    log_warn(logger_tags::GRAPHICS, "Couldn't fullscreen window - no current monitor ?! (@", reinterpret_cast<size_t>(handle), ")");
                    return;
                }

                fullscreen = true;

                const GLFWvidmode* video_mode = glfwGetVideoMode(current_monitor);
                glfwSetWindowMonitor(handle, current_monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);

                fullscreen_width = video_mode->width;
                fullscreen_height = video_mode->height;
                viewport = {0, 0, fullscreen_width, fullscreen_height};

                log_info(logger_tags::GRAPHICS, "Set fullscreen for window @", reinterpret_cast<size_t>(handle), " Res ", fullscreen_width, "x", fullscreen_height);
                break;
            }

            case window_mode::BORDERLESS:
            {
                if (current_monitor == nullptr) current_monitor = glfwGetPrimaryMonitor();
                if (current_monitor == nullptr) return;

                fullscreen = true;

                const GLFWvidmode* video_mode = glfwGetVideoMode(current_monitor);
                i32 window_x, window_y;
                glfwGetWindowPos(handle, &window_x, &window_y);

                fullscreen_width = video_mode->width + 2;
                fullscreen_height = video_mode->height + 2;
                viewport = {0, 0, fullscreen_width, fullscreen_height};

                //If the window size/position exactly covers the display then windows does stupid full screen 'optimizations' that prevent transparent windows working
                //Sooooo.... we make it overlap the edges by a pixel on each side to prevent that.
                glfwSetWindowMonitor(handle, nullptr, window_x - 1, window_y - 1, fullscreen_width, fullscreen_height, GLFW_DONT_CARE);
                log_info(logger_tags::GRAPHICS, "Set borderless fullscreen for window @", reinterpret_cast<size_t>(handle), " Res ", fullscreen_width, "x", fullscreen_height);
                break;
            }

            case window_mode::WINDOWED:
            {
                fullscreen = false;
                glfwSetWindowMonitor(handle, nullptr, x, y, width, height, GLFW_DONT_CARE);
                viewport = {0, 0, width, height};
                log_info(logger_tags::GRAPHICS, "Set windowed for window @", reinterpret_cast<size_t>(handle), " Res ", width, "x", height);
                break;
            }
        }

        update_current_monitor();
    }

    void native_window::set_hidden_cursor() const
    {
        ZoneScoped;
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        log_info(logger_tags::GRAPHICS, "Set Hidden cursor for window @", reinterpret_cast<size_t>(handle));
    }

    void native_window::set_captured_cursor() const
    {
        ZoneScoped;
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        log_info(logger_tags::GRAPHICS, "Set Captured cursor for window @", reinterpret_cast<size_t>(handle));
    }

    void native_window::set_normal_cursor() const
    {
        ZoneScoped;
        glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        log_info(logger_tags::GRAPHICS, "Set Normal cursor for window @", reinterpret_cast<size_t>(handle));
    }

    void native_window::set_vsync(const i32 sync) const
    {
        ZoneScoped;
        this->apply_context();
        glfwSwapInterval(sync);
        log_info(logger_tags::GRAPHICS, "Vsync for window @", reinterpret_cast<size_t>(handle), " set to ", sync);
    }

    void native_window::hide()
    {
        ZoneScoped;
        glfwIconifyWindow(handle);
        is_visible = false;
        log_info(logger_tags::GRAPHICS, "Hidden window @", reinterpret_cast<size_t>(handle));
    }

    void native_window::show()
    {
        ZoneScoped;
        glfwShowWindow(handle);
        is_visible = true;
    }

    void native_window::vanish()
    {
        ZoneScoped;
        glfwHideWindow(handle);
        is_visible = false;
        log_info(logger_tags::GRAPHICS, "Vanished window @", reinterpret_cast<size_t>(handle));
    }

    void native_window::toggle_visible()
    {
        ZoneScoped;
        if (!visible()) show();
        else hide();
    }

    void native_window::close()
    {
        ZoneScoped;
        log_info(logger_tags::GRAPHICS, "Closing window @", reinterpret_cast<size_t>(handle));
        glfwDestroyWindow(handle);
        handle = nullptr;
    }

    void native_window::apply_context() const
    {
        ZoneScoped;
        glfwMakeContextCurrent(handle);
    }

    void native_window::swap_buffers()
    {
        ZoneScoped;
        glfwSwapBuffers(handle);
        frame_count++;
        size_changed = false;
    }

    bool native_window::visible() const
    {
        return is_visible;
    }

    i32 native_window::get_width() const
    {
        return fullscreen ? fullscreen_width : width;
    }

    i32 native_window::get_height() const
    {
        return fullscreen ? fullscreen_height : height;
    }

    glm::uvec2 native_window::get_size() const
    {
        return {width, height};
    }

    glm::uvec2 native_window::get_position() const
    {
        return {x, y};
    }

    f32 native_window::get_aspect() const
    {
        return static_cast<f32>(get_width()) / static_cast<f32>(get_height());
    }

    render_viewport native_window::get_viewport() const
    {
        return viewport;
    }

    bool native_window::get_fullscreen() const
    {
        return fullscreen;
    }

    GLFWwindow* native_window::get_handle() const
    {
        return handle;
    }

    GLFWmonitor* native_window::get_current_monitor() const
    {
        return current_monitor;
    }

    bool native_window::exists() const
    {
        return handle != nullptr;
    }

    bool native_window::should_close() const
    {
        ZoneScoped;
        return glfwWindowShouldClose(handle) != 0;
    }

    u32 native_window::get_frame_count() const
    {
        return frame_count;
    }

    bool native_window::has_size_changed() const
    {
        return size_changed;
    }

    void native_window::update_size(GLFWwindow* window, const i32 width, const i32 height)
    {
        ZoneScoped;
        native_window* local_window = static_cast<native_window*>(glfwGetWindowUserPointer(window));

        if (!local_window->get_fullscreen() || width != local_window->fullscreen_width || height != local_window->fullscreen_height)
        {
            local_window->fullscreen = false;
            local_window->width = width;
            local_window->height = height;
        }
        else
        {
            local_window->fullscreen_width = width;
            local_window->fullscreen_height = height;
        }

        local_window->viewport = {0, 0, static_cast<u32>(width), static_cast<u32>(height)};
        local_window->size_changed = true;
    }

    void native_window::update_position(GLFWwindow* window, const i32 x, const i32 y)
    {
        ZoneScoped;
        native_window* local_window = static_cast<native_window*>(glfwGetWindowUserPointer(window));
        local_window->update_current_monitor();
        if (local_window->get_fullscreen()) return;
        local_window->x = x;
        local_window->y = y;
    }


    void native_window::update_current_monitor()
    {
        ZoneScoped;
        GLFWmonitor* best = nullptr;
        i32 win_x = 0, win_y = 0;
        glfwGetWindowPos(handle, &win_x, &win_y);

        i32 win_width = 0, win_height = 0;
        glfwGetWindowSize(handle, &win_width, &win_height);

        if (fullscreen)
        {
            fullscreen_width = win_width;
            fullscreen_height = win_height;
        }
        else
        {
            width = win_width;
            height = win_height;
            x = win_x;
            y = win_y;
        }

        viewport = {0, 0, static_cast<u32>(win_width), static_cast<u32>(win_height)};

        i32 monitor_count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);

        i32 best_overlap = 0;

        for (i32 i = 0; i < monitor_count; i++)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

            i32 monitor_x = 0, monitor_y = 0;
            glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

            const i32 monitor_width = mode->width;
            const i32 monitor_height = mode->height;

            auto min = [](const i32 x, const i32 y)
            {
                return x < y ? x : y;
            };

            auto max = [](const i32 x, const i32 y)
            {
                return x > y ? x : y;
            };

            const i32 overlap_area =
                max(0, min(win_x + win_width, monitor_x + monitor_width) - max(win_x, monitor_x)) *
                max(0, min(win_y + win_height, monitor_y + monitor_height) - max(win_y, monitor_y));

            if (best_overlap < overlap_area)
            {
                best_overlap = overlap_area;
                best = monitors[i];
            }
        }

        current_monitor = best;
    }
}
