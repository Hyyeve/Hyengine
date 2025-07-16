#include "input.hpp"

#include <array>
#include <tracy/Tracy.hpp>

#include "../core/logger.hpp"

namespace hyengine::input
{
    using namespace glm;
    using namespace hyengine;

    const static std::string logger_tag = "Input";

    virtual_controller disconnected_controller;
    std::array<virtual_controller, 16> controller_hardware;
    std::array<int, 16> player_to_controller_map;

    virtual_keyboard keyboard_hardware = virtual_keyboard();
    virtual_mouse mouse_hardware = virtual_mouse();

    virtual_keyboard& current_keyboard = keyboard_hardware;
    virtual_mouse& current_mouse = mouse_hardware;

    void glfw_key_callback(GLFWwindow* /*window*/, const int key, int /*scancode*/, const int action, int /*mods*/)
    {
        ZoneScoped;
        if (action == GLFW_PRESS) keyboard_hardware.press_key(key);
        if (action == GLFW_RELEASE) keyboard_hardware.release_key(key);
    }

    void glfw_char_callback(GLFWwindow* /*window*/, const unsigned int codepoint)
    {
        ZoneScoped;
        keyboard_hardware.type_character(codepoint);
    }

    void glfw_mouse_button_callback(GLFWwindow* /*window*/, const int button, const int action, int /*mods*/)
    {
        ZoneScoped;
        if (action == GLFW_PRESS) mouse_hardware.click_button(button);
        if (action == GLFW_RELEASE) mouse_hardware.release_button(button);
    }

    void glfw_mouse_scroll_callback(GLFWwindow* /*window*/, const double xoffset, const double yoffset)
    {
        ZoneScoped;
        mouse_hardware.scroll({xoffset, yoffset});
    }

    void glfw_mouse_position_callback(GLFWwindow* window, const double x, const double y)
    {
        ZoneScoped;
        int win_height = 0;
        glfwGetWindowSize(window, nullptr, &win_height); //doing this every single time the mouse is updated probably isn't great, but..
        mouse_hardware.move_absolute(vec2(x, win_height - y));
    }

    void glfw_joystick_connection_callback(const int id, const int event)
    {
        ZoneScoped;
        if (event == GLFW_CONNECTED)
        {
            for (int i = 0; i < 16; i++)
            {
                if (player_to_controller_map[i] == -1)
                {
                    player_to_controller_map[i] = id;
                    break;
                }
            }
        }
        else if (event == GLFW_DISCONNECTED)
        {
            for (int i = 0; i < 16; i++)
            {
                if (player_to_controller_map[i] == id)
                {
                    player_to_controller_map[i] = -1;
                    break;
                }
            }
        }
    }

    void reset_controller_map()
    {
        ZoneScoped;
        for (int i = 0; i < 16; i++) player_to_controller_map[i] = -1;
        for (int i = 0; i < 16; i++)
        {
            if (glfwJoystickPresent(i)) glfw_joystick_connection_callback(i, GLFW_CONNECTED);
        }
    }

    void bind_callbacks(GLFWwindow* window)
    {
        ZoneScoped;
        reset_controller_map();

        glfwSetKeyCallback(window, glfw_key_callback);
        glfwSetCharCallback(window, glfw_char_callback);
        glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
        glfwSetScrollCallback(window, glfw_mouse_scroll_callback);
        glfwSetCursorPosCallback(window, glfw_mouse_position_callback);
        glfwSetJoystickCallback(glfw_joystick_connection_callback);

        logger::info(logger_tag, "Bound hardware input callbacks to window @", reinterpret_cast<size_t>(window));
    }

    void poll_controllers()
    {
        ZoneScoped;
        for (int idx = 0; idx < 16; idx++)
        {
            if (!glfwJoystickPresent(idx)) continue;
            virtual_controller& controller = controller_hardware[idx];

            GLFWgamepadstate gamepad_state;

            if (glfwJoystickIsGamepad(idx) && glfwGetGamepadState(idx, &gamepad_state) == GLFW_TRUE)
            {
                for (int axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; axis++) controller.set_axis(axis, gamepad_state.axes[axis]);
                for (int button = 0; button <= GLFW_GAMEPAD_BUTTON_LAST; button++)
                {
                    const bool pressed = gamepad_state.buttons[button] == GLFW_PRESS;

                    if (pressed && !controller.pressed(button)) controller.press_button(button);
                    else if (!pressed && controller.pressed(button)) controller.release_button(button);

                    virtual_controller::dpad dpad_direction = virtual_controller::dpad::UP;
                    bool is_dpad = false;

                    switch (button)
                    {
                        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
                        {
                            dpad_direction = virtual_controller::dpad::LEFT;
                            is_dpad = true;
                            break;
                        }
                        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
                        {
                            dpad_direction = virtual_controller::dpad::RIGHT;
                            is_dpad = true;
                        }
                        case GLFW_GAMEPAD_BUTTON_DPAD_UP:
                        {
                            dpad_direction = virtual_controller::dpad::UP;
                            is_dpad = true;
                        }
                        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
                        {
                            dpad_direction = virtual_controller::dpad::DOWN;
                            is_dpad = true;
                        }
                        default: break;
                    }

                    if (is_dpad)
                    {
                        if (pressed && !controller.direction_pressed(0, dpad_direction)) controller.press_direction(0, dpad_direction);
                        else if (!pressed && controller.direction_pressed(0, dpad_direction)) controller.release_direction(0, dpad_direction);
                    }
                }
            }
            else
            {
                int buttons_count;
                const unsigned char* buttons = glfwGetJoystickButtons(idx, &buttons_count);
                int axes_count;
                const float* axes = glfwGetJoystickAxes(idx, &axes_count);
                int hats_count;
                const unsigned char* hats = glfwGetJoystickHats(idx, &hats_count);

                for (int i = 0; i < buttons_count; i++)
                {
                    const bool pressed = buttons[i] == GLFW_PRESS;
                    if (pressed && !controller.pressed(i)) controller.press_button(i);
                    else if (!pressed && controller.pressed(i)) controller.release_button(i);
                }

                for (int i = 0; i < axes_count; i++) controller.set_axis(i, axes[i]);

                for (int i = 0; i < hats_count; i++)
                {
                    const bool hat_up = hats[i] & GLFW_HAT_UP;
                    const bool hat_down = hats[i] & GLFW_HAT_DOWN;
                    const bool hat_left = hats[i] & GLFW_HAT_LEFT;
                    const bool hat_right = hats[i] & GLFW_HAT_RIGHT;

                    if (hat_up && !controller.direction_pressed(i, virtual_controller::dpad::UP)) controller.press_direction(i, virtual_controller::dpad::UP);
                    else if (!hat_up && controller.direction_pressed(i, virtual_controller::dpad::UP)) controller.release_direction(i, virtual_controller::dpad::UP);

                    if (hat_down && !controller.direction_pressed(i, virtual_controller::dpad::DOWN)) controller.press_direction(i, virtual_controller::dpad::DOWN);
                    else if (!hat_down && controller.direction_pressed(i, virtual_controller::dpad::DOWN)) controller.release_direction(i, virtual_controller::dpad::DOWN);

                    if (hat_left && !controller.direction_pressed(i, virtual_controller::dpad::LEFT)) controller.press_direction(i, virtual_controller::dpad::LEFT);
                    else if (!hat_left && controller.direction_pressed(i, virtual_controller::dpad::LEFT)) controller.release_direction(i, virtual_controller::dpad::LEFT);

                    if (hat_right && !controller.direction_pressed(i, virtual_controller::dpad::RIGHT)) controller.press_direction(i, virtual_controller::dpad::RIGHT);
                    else if (!hat_right && controller.direction_pressed(i, virtual_controller::dpad::RIGHT)) controller.release_direction(i, virtual_controller::dpad::RIGHT);
                }
            }
        }
    }

    void process_inputs()
    {
        ZoneScoped;
        keyboard_hardware.process_inputs();
        mouse_hardware.process_inputs();
        for (virtual_controller& controller : controller_hardware) controller.process_inputs();
        poll_controllers();
        glfwPollEvents();
    }

    virtual_keyboard& hardware_keyboard()
    {
        ZoneScoped;
        return keyboard_hardware;
    }

    virtual_mouse& hardware_mouse()
    {
        ZoneScoped;
        return mouse_hardware;
    }

    virtual_controller& hardware_controller(const unsigned int player_index)
    {
        ZoneScoped;
        if (player_to_controller_map[player_index] >= 0) return controller_hardware[player_index];
        return disconnected_controller;
    }

    bool hardware_controller_connected(const unsigned int player_index)
    {
        ZoneScoped;
        return player_to_controller_map[player_index] >= 0;
    }

    bool hardware_controller_has_mappings(const unsigned int player_index)
    {
        ZoneScoped;
        if (player_to_controller_map[player_index] >= 0) return glfwJoystickIsGamepad(player_to_controller_map[player_index]);
        return false;
    }

    std::string hardware_controller_name(const unsigned int player_index)
    {
        ZoneScoped;
        const int controller_id = player_to_controller_map[player_index];
        if (controller_id < 0) return "Disconnected Controller";
        if (glfwJoystickIsGamepad(controller_id)) return glfwGetGamepadName(controller_id);
        return glfwGetJoystickName(controller_id);
    }

    void set_keyboard_input(const virtual_keyboard& input)
    {
        ZoneScoped;
        current_keyboard = input;
    }

    void set_mouse_input(const virtual_mouse& input)
    {
        ZoneScoped;
        current_mouse = input;
    }

    void set_hardware_keyboard()
    {
        ZoneScoped;
        current_keyboard = keyboard_hardware;
    }

    void set_hardware_mouse()
    {
        ZoneScoped;
        current_mouse = mouse_hardware;
    }

    bool key_pressed(const int key)
    {
        ZoneScoped;
        return current_keyboard.pressed(key);
    }

    bool key_released(const int key)
    {
        ZoneScoped;
        return current_keyboard.released(key);
    }

    bool key_pressed_this_frame(const int key)
    {
        ZoneScoped;
        return current_keyboard.pressed_this_frame(key);
    }

    bool key_released_this_frame(const int key)
    {
        ZoneScoped;
        return current_keyboard.released_this_frame(key);
    }

    int key_frame_count(const int key)
    {
        ZoneScoped;
        return current_keyboard.frame_count(key);
    }

    int key_pressed_frames(const int key)
    {
        ZoneScoped;
        return current_keyboard.pressed_frames(key);
    }

    int key_released_frames(const int key)
    {
        ZoneScoped;
        return current_keyboard.released_frames(key);
    }

    bool key_control_pressed()
    {
        ZoneScoped;
        return current_keyboard.control_pressed();
    }

    bool key_shift_pressed()
    {
        ZoneScoped;
        return current_keyboard.shift_pressed();
    }

    bool key_alt_pressed()
    {
        ZoneScoped;
        return current_keyboard.alt_pressed();
    }

    bool key_super_pressed()
    {
        ZoneScoped;
        return current_keyboard.super_pressed();
    }

    bool mouse_clicked(const int button)
    {
        ZoneScoped;
        return current_mouse.clicked(button);
    }

    bool mouse_released(const int button)
    {
        ZoneScoped;
        return current_mouse.released(button);
    }

    bool mouse_clicked_this_frame(const int button)
    {
        ZoneScoped;
        return current_mouse.clicked_this_frame(button);
    }

    bool mouse_released_this_frame(const int button)
    {
        ZoneScoped;
        return current_mouse.released_this_frame(button);
    }

    int mouse_frame_count(const int button)
    {
        ZoneScoped;
        return current_mouse.frame_count(button);
    }

    int mouse_pressed_frames(const int button)
    {
        ZoneScoped;
        return current_mouse.pressed_frames(button);
    }

    int mouse_released_frames(const int button)
    {
        ZoneScoped;
        return current_mouse.released_frames(button);
    }

    vec2 mouse_position()
    {
        ZoneScoped;
        return current_mouse.get_position();
    }

    vec2 mouse_position_delta()
    {
        ZoneScoped;
        return current_mouse.get_position_delta();
    }

    vec2 mouse_total_scroll()
    {
        ZoneScoped;
        return current_mouse.get_total_scroll();
    }

    vec2 mouse_scroll_delta()
    {
        ZoneScoped;
        return current_mouse.get_scroll_delta();
    }
}
