#pragma once
#include <string_view>
#include <tracy/Tracy.hpp>

#include "glm/glm.hpp"
#include "Hyengine/core/portability.hpp"

namespace hyengine
{
    constexpr glm::vec4 WHITE = glm::vec4(1, 1, 1, 1);
    constexpr glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
    constexpr glm::vec4 TRANSPARENT_WHITE = glm::vec4(1, 1, 1, 0);
    constexpr glm::vec4 TRANSPARENT_BLACK = glm::vec4(0, 0, 0, 0);

    constexpr glm::vec4 RED = glm::vec4(1, 0, 0, 1);
    constexpr glm::vec4 GREEN = glm::vec4(0, 1, 0, 1);
    constexpr glm::vec4 BLUE = glm::vec4(0, 0, 1, 1);
    constexpr glm::vec4 YELLOW = glm::vec4(1, 1, 0, 1);
    constexpr glm::vec4 CYAN = glm::vec4(0, 1, 1, 1);
    constexpr glm::vec4 MAGENTA = glm::vec4(1, 0, 1, 1);

    constexpr glm::vec4 LIGHT_GRAY = glm::vec4(0.75, 0.75, 0.75, 1);
    constexpr glm::vec4 LIGHT_RED = glm::vec4(1, 0.5, 0.5, 1);
    constexpr glm::vec4 LIGHT_GREEN = glm::vec4(1, 0.5, 0.5, 1);
    constexpr glm::vec4 LIGHT_BLUE = glm::vec4(0.5, 0.5, 1, 1);
    constexpr glm::vec4 LIGHT_YELLOW = glm::vec4(1, 1, 0.5, 1);
    constexpr glm::vec4 LIGHT_CYAN = glm::vec4(0.5, 1, 1, 1);
    constexpr glm::vec4 LIGHT_MAGENTA = glm::vec4(1, 0.5, 1, 1);
    constexpr glm::vec4 LIGHT_ORANGE = glm::vec4(1, 0.75, 0.25, 1);
    constexpr glm::vec4 LIGHT_MINT = glm::vec4(0.25, 1, 0.75, 1);
    constexpr glm::vec4 LIGHT_VIOLET = glm::vec4(0.75, 0.25, 1, 1);
    constexpr glm::vec4 LIGHT_CRIMSON = glm::vec4(1, 0.25, 0.75, 1);
    constexpr glm::vec4 LIGHT_LIME = glm::vec4(0.75, 1, 0.25, 1);
    constexpr glm::vec4 LIGHT_AZURE = glm::vec4(0.25, 0.75, 1, 1);

    constexpr glm::vec4 GRAY = glm::vec4(0.5, 0.5, 0.5, 1);
    constexpr glm::vec4 ORANGE = glm::vec4(1, 0.5, 0, 1);
    constexpr glm::vec4 MINT = glm::vec4(0, 1, 0.5, 1);
    constexpr glm::vec4 VIOLET = glm::vec4(0.5, 0, 1, 1);
    constexpr glm::vec4 CRIMSON = glm::vec4(1, 0, 0.5, 1);
    constexpr glm::vec4 LIME = glm::vec4(0.5, 1, 0, 1);
    constexpr glm::vec4 AZURE = glm::vec4(0, 0.5, 1, 1);


    constexpr glm::vec4 HALF_RED = glm::vec4(0.5, 0, 0, 1);
    constexpr glm::vec4 HALF_GREEN = glm::vec4(0, 0.5, 0, 1);
    constexpr glm::vec4 HALF_BLUE = glm::vec4(0, 0, 0.5, 1);
    constexpr glm::vec4 HALF_YELLOW = glm::vec4(0.5, 0.5, 0, 1);
    constexpr glm::vec4 HALF_CYAN = glm::vec4(0, 0.5, 0.5, 1);
    constexpr glm::vec4 HALF_MAGENTA = glm::vec4(0.5, 0, 0.5, 1);

    constexpr glm::vec4 DARK_GRAY = glm::vec4(0.25, 0.25, 0.25, 1);
    constexpr glm::vec4 DARK_RED = glm::vec4(0.25, 0, 0, 1);
    constexpr glm::vec4 DARK_GREEN = glm::vec4(0, 0.25, 0, 1);
    constexpr glm::vec4 DARK_BLUE = glm::vec4(0, 0, 0.25, 1);
    constexpr glm::vec4 DARK_YELLOW = glm::vec4(0.25, 0.25, 0, 1);
    constexpr glm::vec4 DARK_CYAN = glm::vec4(0, 0.25, 0.25, 1);
    constexpr glm::vec4 DARK_MAGENTA = glm::vec4(0.25, 0, 0.25, 1);
    constexpr glm::vec4 DARK_ORANGE = glm::vec4(0.5, 0.25, 0, 1);
    constexpr glm::vec4 DARK_MINT = glm::vec4(0, 0.5, 0.25, 1);
    constexpr glm::vec4 DARK_VIOLET = glm::vec4(0.25, 0, 0.5, 1);
    constexpr glm::vec4 DARK_CRIMSON = glm::vec4(0.5, 0, 0.25, 1);
    constexpr glm::vec4 DARK_LIME = glm::vec4(0.25, 0.5, 0, 1);
    constexpr glm::vec4 DARK_AZURE = glm::vec4(0, 0.25, 0.5, 1);

    [[nodiscard]] constexpr glm::vec4 bits_to_color(const u32 hex)
    {
        return {
            static_cast<f32>(hex & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 8u & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 16u & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 24u & 0xFFu) / 255.0f,
        };
    }

    [[nodiscard]] constexpr u32 color_to_bits(const glm::vec4 col)
    {
        const u32 byte_r = static_cast<u8>(col.r * 255);
        const u32 byte_g = static_cast<u8>(col.g * 255);
        const u32 byte_b = static_cast<u8>(col.b * 255);
        const u32 byte_a = static_cast<u8>(col.a * 255);
        return byte_a << 24u | (byte_b << 16u) | (byte_g << 8u) | byte_r;
    }

    [[nodiscard]] inline glm::vec4 random()
    {
        return bits_to_color(rand());
    }

    [[nodiscard]] constexpr glm::vec4 from_name(const std::string_view name)
    {
        switch (string_hash(name))
        {
            case string_hash("white"): return WHITE;
            case string_hash("black"): return BLACK;
            case string_hash("transparent_white"): return TRANSPARENT_WHITE;
            case string_hash("transparent_black"): return TRANSPARENT_BLACK;

            case string_hash("red"): return RED;
            case string_hash("green"): return GREEN;
            case string_hash("blue"): return BLUE;
            case string_hash("yellow"): return YELLOW;
            case string_hash("cyan"): return CYAN;
            case string_hash("magenta"): return MAGENTA;

            case string_hash("light_gray"): return LIGHT_GRAY;
            case string_hash("light_red"): return LIGHT_RED;
            case string_hash("light_green"): return LIGHT_GREEN;
            case string_hash("light_blue"): return LIGHT_BLUE;
            case string_hash("light_yellow"): return LIGHT_YELLOW;
            case string_hash("light_cyan"): return LIGHT_CYAN;
            case string_hash("light_magenta"): return LIGHT_MAGENTA;
            case string_hash("light_orange"): return LIGHT_ORANGE;
            case string_hash("light_mi32"): return LIGHT_MINT;
            case string_hash("light_violet"): return LIGHT_VIOLET;
            case string_hash("light_crimson"): return LIGHT_CRIMSON;
            case string_hash("light_lime"): return LIGHT_LIME;
            case string_hash("light_azure"): return LIGHT_AZURE;

            case string_hash("gray"): return GRAY;
            case string_hash("orange"): return ORANGE;
            case string_hash("mi32"): return MINT;
            case string_hash("violet"): return VIOLET;
            case string_hash("crimson"): return CRIMSON;
            case string_hash("lime"): return LIME;
            case string_hash("azure"): return AZURE;

            case string_hash("half_red"): return HALF_RED;
            case string_hash("half_green"): return HALF_GREEN;
            case string_hash("half_blue"): return HALF_BLUE;
            case string_hash("half_yellow"): return HALF_YELLOW;
            case string_hash("half_cyan"): return HALF_CYAN;
            case string_hash("half_magenta"): return HALF_MAGENTA;

            case string_hash("dark_gray"): return DARK_GRAY;
            case string_hash("dark_red"): return DARK_RED;
            case string_hash("dark_green"): return DARK_GREEN;
            case string_hash("dark_blue"): return DARK_BLUE;
            case string_hash("dark_yellow"): return DARK_YELLOW;
            case string_hash("dark_cyan"): return DARK_CYAN;
            case string_hash("dark_magenta"): return DARK_MAGENTA;
            case string_hash("dark_orange"): return DARK_ORANGE;
            case string_hash("dark_mi32"): return DARK_MINT;
            case string_hash("dark_violet"): return DARK_VIOLET;
            case string_hash("dark_crimson"): return DARK_CRIMSON;
            case string_hash("dark_lime"): return DARK_LIME;
            case string_hash("dark_azure"): return DARK_AZURE;

            default: return TRANSPARENT_BLACK;
        }
    }
}
