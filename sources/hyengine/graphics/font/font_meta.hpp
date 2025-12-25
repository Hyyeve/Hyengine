#pragma once
#include <string>
#include <string_view>
#include "../../library/glm.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    struct font_glyph_meta
    {
        float advance;
        glm::vec4 cursor_relative_bounds;
        glm::vec4 atlas_pixel_bounds;
    };

    class font_meta
    {
    public:
        explicit font_meta(const std::string_view& asset_id) noexcept;

        font_meta(font_meta&& other) noexcept = delete;
        font_meta(const font_meta& other) noexcept = delete;
        font_meta& operator=(const font_meta& other) noexcept = delete;
        font_meta& operator=(font_meta&& other) noexcept = delete;

        ~font_meta();

        void load();
        void unload();
        void reload();

        [[nodiscard]] font_glyph_meta get_glyph_data(const u32 codepoint);
        [[nodiscard]] std::string get_asset_id() const;

    private:
        std::unordered_map<u32, font_glyph_meta> glyphs;
        std::string asset_id;
    };
}
