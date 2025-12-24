#pragma once
#include "../library/glm.hpp"
#include <map>

#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    class virtual_mouse
    {
    public:
        void move_relative(glm::vec2 delta);
        void move_absolute(glm::vec2 position);
        void scroll(glm::vec2 delta);
        void click_button(i32 button);
        void release_button(i32 button);

        [[nodiscard]] bool clicked(i32 button);
        [[nodiscard]] bool released(i32 button);
        [[nodiscard]] bool clicked_this_frame(i32 button);
        [[nodiscard]] bool released_this_frame(i32 button);

        [[nodiscard]] i32 frame_count(i32 button);
        [[nodiscard]] i32 pressed_frames(i32 button);
        [[nodiscard]] i32 released_frames(i32 button);

        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] glm::vec2 get_position_delta() const;
        [[nodiscard]] glm::vec2 get_total_scroll() const;
        [[nodiscard]] glm::vec2 get_scroll_delta() const;

        void process_inputs();

    private:
        std::map<i32, i32> button_map = std::map<i32, i32>();

        glm::vec2 mouse_position = glm::vec2();
        glm::vec2 mouse_delta = glm::vec2();
        glm::vec2 total_scroll = glm::vec2();
        glm::vec2 scroll_delta = glm::vec2();
    };
}
