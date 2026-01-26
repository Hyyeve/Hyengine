#pragma once
#include <string_view>
#include <tracy/Tracy.hpp>

#include "glm/glm.hpp"
#include "../common/common.hpp"
#include "hyengine/core/hyengine.hpp"

namespace hyengine
{
    namespace colors
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
    }

    ///Converts hex 0xRRGGBBAA values to 4-channel RGBA float
    [[nodiscard]] constexpr glm::vec4 bits_to_color(const u32 hex)
    {
        return {
            static_cast<f32>(hex >> 24u & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 16u & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 8u & 0xFFu) / 255.0f,
            static_cast<f32>(hex >> 0u & 0xFFu) / 255.0f,
        };
    }

    ///Converts 4-channel RGBA float to a hex 0xRRGGBBAA value
    [[nodiscard]] constexpr u32 color_to_bits(const glm::vec4 col)
    {
        const u32 byte_r = static_cast<u8>(col.r * 255);
        const u32 byte_g = static_cast<u8>(col.g * 255);
        const u32 byte_b = static_cast<u8>(col.b * 255);
        const u32 byte_a = static_cast<u8>(col.a * 255);
        return byte_r << 24u | (byte_g << 16u) | (byte_b << 8u) | byte_a;
    }

    ///Converts the bits of a random u32 into a color
    [[nodiscard]] inline glm::vec4 random_color()
    {
        return bits_to_color(random());
    }

    ///Returns the value of a named color constant by name
    [[nodiscard]] constexpr glm::vec4 color_from_name(const std::string_view name)
    {
        switch (string_hash(name))
        {
            case string_hash("white"): return colors::WHITE;
            case string_hash("black"): return colors::BLACK;
            case string_hash("transparent_white"): return colors::TRANSPARENT_WHITE;
            case string_hash("transparent_black"): return colors::TRANSPARENT_BLACK;

            case string_hash("red"): return colors::RED;
            case string_hash("green"): return colors::GREEN;
            case string_hash("blue"): return colors::BLUE;
            case string_hash("yellow"): return colors::YELLOW;
            case string_hash("cyan"): return colors::CYAN;
            case string_hash("magenta"): return colors::MAGENTA;

            case string_hash("light_gray"): return colors::LIGHT_GRAY;
            case string_hash("light_red"): return colors::LIGHT_RED;
            case string_hash("light_green"): return colors::LIGHT_GREEN;
            case string_hash("light_blue"): return colors::LIGHT_BLUE;
            case string_hash("light_yellow"): return colors::LIGHT_YELLOW;
            case string_hash("light_cyan"): return colors::LIGHT_CYAN;
            case string_hash("light_magenta"): return colors::LIGHT_MAGENTA;
            case string_hash("light_orange"): return colors::LIGHT_ORANGE;
            case string_hash("light_mi32"): return colors::LIGHT_MINT;
            case string_hash("light_violet"): return colors::LIGHT_VIOLET;
            case string_hash("light_crimson"): return colors::LIGHT_CRIMSON;
            case string_hash("light_lime"): return colors::LIGHT_LIME;
            case string_hash("light_azure"): return colors::LIGHT_AZURE;

            case string_hash("gray"): return colors::GRAY;
            case string_hash("orange"): return colors::ORANGE;
            case string_hash("mi32"): return colors::MINT;
            case string_hash("violet"): return colors::VIOLET;
            case string_hash("crimson"): return colors::CRIMSON;
            case string_hash("lime"): return colors::LIME;
            case string_hash("azure"): return colors::AZURE;

            case string_hash("half_red"): return colors::HALF_RED;
            case string_hash("half_green"): return colors::HALF_GREEN;
            case string_hash("half_blue"): return colors::HALF_BLUE;
            case string_hash("half_yellow"): return colors::HALF_YELLOW;
            case string_hash("half_cyan"): return colors::HALF_CYAN;
            case string_hash("half_magenta"): return colors::HALF_MAGENTA;

            case string_hash("dark_gray"): return colors::DARK_GRAY;
            case string_hash("dark_red"): return colors::DARK_RED;
            case string_hash("dark_green"): return colors::DARK_GREEN;
            case string_hash("dark_blue"): return colors::DARK_BLUE;
            case string_hash("dark_yellow"): return colors::DARK_YELLOW;
            case string_hash("dark_cyan"): return colors::DARK_CYAN;
            case string_hash("dark_magenta"): return colors::DARK_MAGENTA;
            case string_hash("dark_orange"): return colors::DARK_ORANGE;
            case string_hash("dark_mi32"): return colors::DARK_MINT;
            case string_hash("dark_violet"): return colors::DARK_VIOLET;
            case string_hash("dark_crimson"): return colors::DARK_CRIMSON;
            case string_hash("dark_lime"): return colors::DARK_LIME;
            case string_hash("dark_azure"): return colors::DARK_AZURE;

            default: return colors::TRANSPARENT_BLACK;
        }
    }
}
