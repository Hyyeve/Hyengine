#pragma once
#include "font_meta.hpp"
#include <ranges>

#include "Hyengine/core/file_io.hpp"
#include "Hyengine/core/portability.hpp"
#include "Hyengine/graphics/renderers/basic_renderer.hpp"

namespace hyengine
{
    font_meta::font_meta(const std::string_view& asset_id) noexcept : asset_id(asset_id)
    {
    }

    font_meta::~font_meta()
    {
        unload();
    }

    void font_meta::load()
    {
        const std::string raw_csv = load_asset_text(asset_id);
        if (raw_csv.empty())
        {
            log_error(logger_tag, "Failed to load font metadata \"", asset_id, "\" - got no glyph data.");
            return;
        }

        std::istringstream text(raw_csv);
        std::string line;
        while (std::getline(text, line))
        {
            if (line.empty()) continue;
            std::istringstream line_stream(line);
            std::string datapoint;
            std::getline(line_stream, datapoint, ',');
            u32 codepoint = parse_u32(datapoint, 0);
            std::getline(line_stream, datapoint, ',');
            f32 advance = parse_f32(datapoint, 1.0f);
            std::getline(line_stream, datapoint, ',');
            f32 em_bounds_left = parse_f32(datapoint, 0.0f);
            std::getline(line_stream, datapoint, ',');
            f32 em_bounds_bottom = parse_f32(datapoint, 0.0f);
            std::getline(line_stream, datapoint, ',');
            f32 em_bounds_right = parse_f32(datapoint, 1.0f);
            std::getline(line_stream, datapoint, ',');
            f32 em_bounds_top = parse_f32(datapoint, 1.0f);
            std::getline(line_stream, datapoint, ',');
            f32 atlas_bounds_left = parse_f32(datapoint, 0.0f);
            std::getline(line_stream, datapoint, ',');
            f32 atlas_bounds_bottom = parse_f32(datapoint, 0.0f);
            std::getline(line_stream, datapoint, ',');
            f32 atlas_bounds_right = parse_f32(datapoint, 1.0f);
            std::getline(line_stream, datapoint, ',');
            f32 atlas_bounds_top = parse_f32(datapoint, 1.0f);

            font_glyph_meta glyph_meta = {
                advance,
                glm::vec4(em_bounds_left, em_bounds_bottom, em_bounds_right, em_bounds_top),
                glm::vec4(atlas_bounds_left, atlas_bounds_bottom, atlas_bounds_right, atlas_bounds_top)
            };

            glyphs[codepoint] = glyph_meta;
        }

        log_info(logger_tag, "Loaded font metadata \"", asset_id, "\" - got ", stringify_count(glyphs.size(), "glyph"), ".");
    }

    void font_meta::unload()
    {
        glyphs.clear();
    }

    void font_meta::reload()
    {
        unload();
        load();
    }

    font_glyph_meta font_meta::get_glyph_data(const u32 codepoint)
    {
        if (!glyphs.contains(codepoint)) return {};
        return glyphs[codepoint];
    }

    std::string font_meta::get_asset_id() const
    {
        return asset_id;
    }
}
