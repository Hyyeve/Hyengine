#pragma once
#include <string_view>
#include <tracy/Tracy.hpp>

#include "glm/glm.hpp"

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
        auto hash = [](const std::string_view string) noexcept
        {
            u32 hash_value = 5381;
            for (const char c : string) hash_value = (hash_value << 5u) + hash_value + static_cast<unsigned char>(c);
            return hash_value;
        };

        switch (hash(name))
        {
            case hash("white"): return WHITE;
            case hash("black"): return BLACK;
            case hash("transparent_white"): return TRANSPARENT_WHITE;
            case hash("transparent_black"): return TRANSPARENT_BLACK;

            case hash("red"): return RED;
            case hash("green"): return GREEN;
            case hash("blue"): return BLUE;
            case hash("yellow"): return YELLOW;
            case hash("cyan"): return CYAN;
            case hash("magenta"): return MAGENTA;

            case hash("light_gray"): return LIGHT_GRAY;
            case hash("light_red"): return LIGHT_RED;
            case hash("light_green"): return LIGHT_GREEN;
            case hash("light_blue"): return LIGHT_BLUE;
            case hash("light_yellow"): return LIGHT_YELLOW;
            case hash("light_cyan"): return LIGHT_CYAN;
            case hash("light_magenta"): return LIGHT_MAGENTA;
            case hash("light_orange"): return LIGHT_ORANGE;
            case hash("light_mi32"): return LIGHT_MINT;
            case hash("light_violet"): return LIGHT_VIOLET;
            case hash("light_crimson"): return LIGHT_CRIMSON;
            case hash("light_lime"): return LIGHT_LIME;
            case hash("light_azure"): return LIGHT_AZURE;

            case hash("gray"): return GRAY;
            case hash("orange"): return ORANGE;
            case hash("mi32"): return MINT;
            case hash("violet"): return VIOLET;
            case hash("crimson"): return CRIMSON;
            case hash("lime"): return LIME;
            case hash("azure"): return AZURE;

            case hash("half_red"): return HALF_RED;
            case hash("half_green"): return HALF_GREEN;
            case hash("half_blue"): return HALF_BLUE;
            case hash("half_yellow"): return HALF_YELLOW;
            case hash("half_cyan"): return HALF_CYAN;
            case hash("half_magenta"): return HALF_MAGENTA;

            case hash("dark_gray"): return DARK_GRAY;
            case hash("dark_red"): return DARK_RED;
            case hash("dark_green"): return DARK_GREEN;
            case hash("dark_blue"): return DARK_BLUE;
            case hash("dark_yellow"): return DARK_YELLOW;
            case hash("dark_cyan"): return DARK_CYAN;
            case hash("dark_magenta"): return DARK_MAGENTA;
            case hash("dark_orange"): return DARK_ORANGE;
            case hash("dark_mi32"): return DARK_MINT;
            case hash("dark_violet"): return DARK_VIOLET;
            case hash("dark_crimson"): return DARK_CRIMSON;
            case hash("dark_lime"): return DARK_LIME;
            case hash("dark_azure"): return DARK_AZURE;

            default: return TRANSPARENT_BLACK;
        }
    }
}
