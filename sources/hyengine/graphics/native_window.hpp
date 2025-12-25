#pragma once

#include "graphics.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    enum class window_mode
    {
        WINDOWED, FULLSCREEN, BORDERLESS
    };

    struct window_config
    {
        u32 width = 1280;
        u32 height = 720;

        std::string title = "App Window";
        GLFWimage* icon = nullptr;

        window_mode mode = window_mode::WINDOWED;

        bool vsync = true;
        bool decorated = true;
        bool resizable = false;
        bool transparent = false;
        bool visible = true;

        f32 opacity = 1.0f;

        u32 gl_version_major = 4;
        u32 gl_version_minor = 5;
        bool gl_profile_compatibility = false;
        bool gl_profile_debug = false;
    };

    class native_window
    {
    public:
        explicit native_window(const window_config& config, const native_window* parent = nullptr);

        ~native_window();


        //windows should not be handled as value types
        native_window(native_window& other) = delete; //COPY CONSTRUCTOR
        native_window(native_window&& other) = delete; //MOVE CONSTRUCTOR
        native_window& operator=(native_window& other) = delete; //COPY ASSIGNMENT
        native_window& operator=(native_window&& other) noexcept = delete; //MOVE ASSIGNMENT

        void set_title(const char* title) const;
        void set_icon(const GLFWimage* icon) const;
        void set_opacity(const f32 opacity) const;
        void set_size(i32 width, i32 height);
        void set_position(i32 x, i32 y);
        void set_decorated(bool decorations) const;
        void center_position();

        void set_window_mode(const window_mode mode);

        void set_hidden_cursor() const;
        void set_captured_cursor() const;
        void set_normal_cursor() const;

        void set_vsync(i32 sync) const;

        void hide();
        void show();
        void vanish();
        void toggle_visible();
        void close();

        void apply_context() const;
        void swap_buffers();

        [[nodiscard]] bool visible() const;
        [[nodiscard]] i32 get_width() const;
        [[nodiscard]] i32 get_height() const;
        [[nodiscard]] glm::uvec2 get_size() const;
        [[nodiscard]] glm::uvec2 get_position() const;
        [[nodiscard]] f32 get_aspect() const;
        [[nodiscard]] render_viewport get_viewport() const;
        [[nodiscard]] bool get_fullscreen() const;

        [[nodiscard]] GLFWwindow* get_handle() const;
        [[nodiscard]] GLFWmonitor* get_current_monitor() const;

        [[nodiscard]] bool exists() const;
        [[nodiscard]] bool should_close() const;

        [[nodiscard]] u32 get_frame_count() const;

        [[nodiscard]] bool has_size_changed() const;

    private:
        GLFWwindow* handle = nullptr;
        GLFWmonitor* current_monitor = nullptr;

        i32 x = 0;
        i32 y = 0;

        u32 width = 0;
        u32 height = 0;

        u32 fullscreen_width = 0;
        u32 fullscreen_height = 0;

        unsigned long frame_count = 0;
        f64 creation_time = 0;
        bool fullscreen = false;
        bool decorated = true;
        bool transparent = false;
        bool size_changed = true;
        bool is_visible = true;
        render_viewport viewport = {0, 0, 0, 0};

        static void update_size(GLFWwindow* window, i32 width, i32 height);
        static void update_position(GLFWwindow* window, i32 x, i32 y);
        void update_current_monitor();
    };
}
