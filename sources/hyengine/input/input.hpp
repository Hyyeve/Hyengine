#pragma once

#include "virtual_controller.hpp"
#include "../library/gl.hpp"
#include "../library/glm.hpp"
#include "virtual_keyboard.hpp"
#include "virtual_mouse.hpp"

namespace hyengine
{
    using namespace hyengine;

    void bind_input(GLFWwindow* window);
    void process_inputs();

    void set_keyboard_input(const virtual_keyboard& input);
    void set_mouse_input(const virtual_mouse& input);

    void set_hardware_keyboard();
    void set_hardware_mouse();

    [[nodiscard]] virtual_keyboard& hardware_keyboard();
    [[nodiscard]] virtual_mouse& hardware_mouse();
    [[nodiscard]] virtual_controller& hardware_controller(u32 player_index);
    [[nodiscard]] bool hardware_controller_connected(u32 player_index);
    [[nodiscard]] bool hardware_controller_has_mappings(u32 player_index);
    void hardware_controller_swap_player_slots(u32 player_index_a, u32 player_index_b);
    [[nodiscard]] std::string hardware_controller_name(u32 player_index);

    [[nodiscard]] bool key_pressed(i32 key);
    [[nodiscard]] bool key_released(i32 key);
    [[nodiscard]] bool key_pressed_this_frame(i32 key);
    [[nodiscard]] bool key_released_this_frame(i32 key);

    [[nodiscard]] i32 key_frame_count(i32 key);
    [[nodiscard]] i32 key_pressed_frames(i32 key);
    [[nodiscard]] i32 key_released_frames(i32 key);

    [[nodiscard]] bool key_control_pressed();
    [[nodiscard]] bool key_shift_pressed();
    [[nodiscard]] bool key_alt_pressed();
    [[nodiscard]] bool key_super_pressed();

    [[nodiscard]] bool mouse_clicked(i32 button);
    [[nodiscard]] bool mouse_released(i32 button);
    [[nodiscard]] bool mouse_clicked_this_frame(i32 button);
    [[nodiscard]] bool mouse_released_this_frame(i32 button);

    [[nodiscard]] i32 mouse_frame_count(i32 button);
    [[nodiscard]] i32 mouse_pressed_frames(i32 button);
    [[nodiscard]] i32 mouse_released_frames(i32 button);

    [[nodiscard]] glm::vec2 mouse_position();
    [[nodiscard]] glm::vec2 mouse_position_delta();
    [[nodiscard]] glm::vec2 mouse_total_scroll();
    [[nodiscard]] glm::vec2 mouse_scroll_delta();
}
