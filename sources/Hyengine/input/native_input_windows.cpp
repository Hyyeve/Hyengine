#include <set>
#include <tracy/Tracy.hpp>

#include "windows.h"
#include "../graphics/native_window.hpp"
#include "native_input.hpp"
#include "../core/logger.hpp"

namespace hyengine::input::native {

    unsigned int global_hotkey_id = 1;

    std::set<unsigned int> pressed_hotkeys;

    unsigned int glfw_to_native_modifiers(const unsigned int mods)
    {
        ZoneScoped;
        unsigned int result = 0;
        if (mods & GLFW_MOD_SHIFT) result |= MOD_SHIFT;
        if (mods & GLFW_MOD_CONTROL) result |= MOD_CONTROL;
        if (mods & GLFW_MOD_ALT) result |= MOD_ALT;
        return result;
    }

    unsigned int glfw_to_native_key(const unsigned int key)
    {
        ZoneScoped;
        if (key == GLFW_KEY_SPACE) return key; //space matches
        if (key <= GLFW_KEY_SLASH) return key + 149; //apostrophe through slash are same order at a much higher windows code
        if (key == GLFW_KEY_SEMICOLON) return VK_OEM_1;
        if (key == GLFW_KEY_EQUAL) return VK_OEM_PLUS;
        if (key <= GLFW_KEY_Z) return key; //alphanumerics are standard ascii
        if (key <= GLFW_KEY_GRAVE_ACCENT) return key + 128; //misc brackets etc
        switch (key) //these are a mess and not in a very consistent order
        {
            case GLFW_KEY_ESCAPE: return VK_ESCAPE;
            case GLFW_KEY_ENTER: return VK_RETURN;
            case GLFW_KEY_TAB: return VK_TAB;
            case GLFW_KEY_BACKSPACE: return VK_BACK;
            case GLFW_KEY_INSERT: return VK_INSERT;
            case GLFW_KEY_DELETE: return VK_DELETE;
            case GLFW_KEY_LEFT: return VK_LEFT;
            case GLFW_KEY_RIGHT: return VK_RIGHT;
            case GLFW_KEY_UP: return VK_UP;
            case GLFW_KEY_DOWN: return VK_DOWN;
            case GLFW_KEY_PAGE_UP: return VK_PRIOR;
            case GLFW_KEY_PAGE_DOWN: return VK_NEXT;
            case GLFW_KEY_HOME: return VK_HOME;
            case GLFW_KEY_END: return VK_END;
            case GLFW_KEY_CAPS_LOCK: return VK_CAPITAL;
            case GLFW_KEY_NUM_LOCK: return VK_NUMLOCK;
            case GLFW_KEY_PRINT_SCREEN: return VK_SNAPSHOT;
            case GLFW_KEY_PAUSE: return VK_PAUSE;
            default: break;
        }
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F24) return key - 178; //function keys are offset backwards a bunch
        if (key <= GLFW_KEY_KP_9) return key - 224; //numpad digits
        switch (key)
        {
            case GLFW_KEY_KP_DECIMAL: return VK_DECIMAL;
            case GLFW_KEY_KP_DIVIDE: return VK_DIVIDE;
            case GLFW_KEY_KP_MULTIPLY: return VK_MULTIPLY;
            case GLFW_KEY_KP_SUBTRACT: return VK_SUBTRACT;
            case GLFW_KEY_KP_ADD: return VK_ADD;
            case GLFW_KEY_KP_ENTER: return VK_RETURN;
            default: break;
        }
        switch (key)
        {
            case GLFW_KEY_LEFT_SHIFT: return VK_LSHIFT;
            case GLFW_KEY_LEFT_CONTROL: return VK_LCONTROL;
            case GLFW_KEY_LEFT_ALT: return VK_LMENU;
            case GLFW_KEY_LEFT_SUPER: return VK_LWIN;
            case GLFW_KEY_RIGHT_SHIFT: return VK_RSHIFT;
            case GLFW_KEY_RIGHT_CONTROL: return VK_RCONTROL;
            case GLFW_KEY_RIGHT_ALT: return VK_RMENU;
            case GLFW_KEY_RIGHT_SUPER: return VK_RWIN;
            case GLFW_KEY_MENU: return VK_MENU;
            default: break;
        }
        return 0;
    }

    unsigned int set_global_hotkey(const unsigned int key, const unsigned int mods) {
        ZoneScoped;
        const unsigned int id = global_hotkey_id;

        const unsigned int vk_mods = glfw_to_native_modifiers(mods);
        const unsigned int vk_key = glfw_to_native_key(key);
        const bool success = RegisterHotKey(nullptr, id, vk_mods, vk_key);

        if (success)
        {
            global_hotkey_id++;
            return id;
        }

        return 0;
    }

    void clear_global_hotkey(const unsigned int id)
    {
        ZoneScoped;
        UnregisterHotKey(nullptr, id);
    }

    void setup_event_hook()
    {

    }

    void process_native_input()
    {
        ZoneScoped;
        pressed_hotkeys.clear();
        MSG msg;
        while (PeekMessage(&msg, nullptr, WM_HOTKEY, WM_HOTKEY, PM_REMOVE))
        {
            pressed_hotkeys.emplace(msg.wParam);
        }
    }

    bool global_hotkey_pressed_this_frame(const unsigned int id)
    {
        ZoneScoped;
        return pressed_hotkeys.contains(id);
    }
}
