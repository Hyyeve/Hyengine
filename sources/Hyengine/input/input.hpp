#pragma once

#include "virtual_controller.hpp"
#include "../library/gl.hpp"
#include "../library/glm.hpp"
#include "virtual_keyboard.hpp"
#include "virtual_mouse.hpp"

namespace hyengine::input {
    using namespace hyengine;

    void bind_callbacks(GLFWwindow* window);
    void process_inputs();

    void set_keyboard_input(const virtual_keyboard& input);
    void set_mouse_input(const virtual_mouse& input);

    void set_hardware_keyboard();
    void set_hardware_mouse();

    [[nodiscard]] virtual_keyboard& hardware_keyboard();
    [[nodiscard]] virtual_mouse& hardware_mouse();
    [[nodiscard]] virtual_controller& hardware_controller(unsigned int player_index);
    [[nodiscard]] bool hardware_controller_connected(unsigned int player_index);
    [[nodiscard]] bool hardware_controller_has_mappings(unsigned int player_index);
    [[nodiscard]] std::string hardware_controller_name(unsigned int player_index);

    [[nodiscard]] bool key_pressed(int key);
    [[nodiscard]] bool key_released(int key);
    [[nodiscard]] bool key_pressed_this_frame(int key);
    [[nodiscard]] bool key_released_this_frame(int key);

    [[nodiscard]] int key_frame_count(int key);
    [[nodiscard]] int key_pressed_frames(int key);
    [[nodiscard]] int key_released_frames(int key);

    [[nodiscard]] bool key_control_pressed();
    [[nodiscard]] bool key_shift_pressed();
    [[nodiscard]] bool key_alt_pressed();
    [[nodiscard]] bool key_super_pressed();

    [[nodiscard]] bool mouse_clicked(int button);
    [[nodiscard]] bool mouse_released(int button);
    [[nodiscard]] bool mouse_clicked_this_frame(int button);
    [[nodiscard]] bool mouse_released_this_frame(int button);

    [[nodiscard]] int mouse_frame_count(int button);
    [[nodiscard]] int mouse_pressed_frames(int button);
    [[nodiscard]] int mouse_released_frames(int button);

    [[nodiscard]] glm::vec2 mouse_position();
    [[nodiscard]] glm::vec2 mouse_position_delta();
    [[nodiscard]] glm::vec2 mouse_total_scroll();
    [[nodiscard]] glm::vec2 mouse_scroll_delta();
}