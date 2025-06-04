#pragma once

#include "graphics.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    enum class window_mode
    {
        WINDOWED, FULLSCREEN, BORDERLESS
    };

    struct window_config
    {
        unsigned int width = 1280;
        unsigned int height = 720;

        std::string title = "App Window";
        GLFWimage* icon = nullptr;

        window_mode mode = window_mode::WINDOWED;

        bool vsync = true;
        bool decorated = true;
        bool resizable = false;
        bool transparent = false;
        bool visible = true;

        float opacity = 1.0f;

        unsigned int gl_version_major = 4;
        unsigned int gl_version_minor = 5;
        bool gl_profile_compatibility = false;
        bool gl_profile_debug = false;
    };

    class native_window {
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
        void set_opacity(const float opacity) const;
        void set_size(int width, int height);
        void set_position(int x, int y);
        void set_decorated(bool decorations) const;
        void center_position();

        void set_window_mode(const window_mode mode);

        void set_hidden_cursor() const;
        void set_captured_cursor() const;
        void set_normal_cursor() const;

        void set_vsync(int sync) const;

        void hide();
        void show();
        void toggle_visible();
        void close();

        void apply_context() const;
        void swap_buffers();

        [[nodiscard]] bool visible() const;
        [[nodiscard]] int get_width() const;
        [[nodiscard]] int get_height() const;
        [[nodiscard]] glm::uvec2 get_size() const;
        [[nodiscard]] glm::uvec2 get_position() const;
        [[nodiscard]] float get_aspect() const;
        [[nodiscard]] viewport get_viewport() const;
        [[nodiscard]] bool get_fullscreen() const;

        [[nodiscard]] GLFWwindow* get_handle() const;
        [[nodiscard]] GLFWmonitor* get_current_monitor() const;

        [[nodiscard]] bool exists() const;
        [[nodiscard]] bool should_close() const;

        [[nodiscard]] unsigned int get_fps() const;
        [[nodiscard]] unsigned int get_frame_count() const;

        [[nodiscard]] bool has_size_changed() const;


    private:
        GLFWwindow* handle = nullptr;
        GLFWmonitor* current_monitor = nullptr;

        int x = 0;
        int y = 0;

        unsigned int width = 0;
        unsigned int height = 0;

        unsigned int fullscreen_width = 0;
        unsigned int fullscreen_height = 0;

        unsigned long frame_count = 0;
        double creation_time = 0;
        bool fullscreen = false;
        bool decorated = true;
        bool transparent = false;
        bool size_changed = true;
        bool is_visible = true;
        viewport viewport = {0, 0, 0, 0};

        const std::string logger_tag = "OS Window";

        static void update_size(GLFWwindow* window, int width, int height);
        static void update_position(GLFWwindow* window, int x, int y);
        void update_current_monitor();
    };
}