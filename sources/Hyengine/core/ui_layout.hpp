#pragma once
#include <array>
#include <limits>
#include <vector>

#include "Hyengine/common/sized_numerics.hpp"


namespace hyengine
{
    struct ui_range
    {
        static ui_range fixed(const u32 value)
        {
            return {value, value, value};
        }

        static ui_range automatic()
        {
            return automatic(std::numeric_limits<u32>::min(), std::numeric_limits<u32>::max());
        }

        static ui_range automatic(const u32 min)
        {
            return automatic(min, std::numeric_limits<u32>::max());
        }

        static ui_range automatic(const u32 min, const u32 max)
        {
            return {min, min, max};
        }

        u32 current;
        u32 min;
        u32 max;
    };

    struct ui_padding
    {
        u32 before;
        u32 after;
    };

    struct ui_axis
    {
        ui_range positioning;
        ui_range sizing;
        ui_padding padding;
        f32 content_alignment; //0-1 percentage

        //Automatically calculated by layout functions
        u32 computed_absolute_position;
        u32 computed_content_size_min;
    };

    struct ui_object
    {
        std::array<ui_axis, 3> transform = {};

        u32 layout_axis;
        u32 content_spacing;

        i32 sizing_priority = 0; //Higher sizing priority will try and match desired size first
        std::vector<ui_object> content;
    };

    void layout_ui(ui_object& root);
}
