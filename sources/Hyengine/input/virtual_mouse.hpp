#pragma once
#include "../library/glm.hpp"
#include <functional>
#include <map>
#include <vector>
namespace hyengine::input {

    class virtual_mouse {
    public:
        void move_relative(glm::vec2 delta);
        void move_absolute(glm::vec2 position);
        void scroll(glm::vec2 delta);
        void click_button(int button);
        void release_button(int button);

        [[nodiscard]] bool clicked(int button);
        [[nodiscard]] bool released(int button);
        [[nodiscard]] bool clicked_this_frame(int button);
        [[nodiscard]] bool released_this_frame(int button);

        [[nodiscard]] int frame_count(int button);
        [[nodiscard]] int pressed_frames(int button);
        [[nodiscard]] int released_frames(int button);

        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] glm::vec2 get_position_delta() const;
        [[nodiscard]] glm::vec2 get_total_scroll() const;
        [[nodiscard]] glm::vec2 get_scroll_delta() const;

        void process_inputs();

    private:
        std::map<int, int> button_map = std::map<int, int>();

        glm::vec2 mouse_position = glm::vec2();
        glm::vec2 mouse_delta = glm::vec2();
        glm::vec2 total_scroll = glm::vec2();
        glm::vec2 scroll_delta = glm::vec2();
    };
}

