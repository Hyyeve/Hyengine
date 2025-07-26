#pragma once
#include <array>
#include <limits>
#include <vector>


namespace hyengine
{
    struct ui_range
    {
        static ui_range fixed(const unsigned int value)
        {
            return {value, value, value};
        }

        static ui_range automatic()
        {
            return automatic(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
        }

        static ui_range automatic(const unsigned int min)
        {
            return automatic(min, std::numeric_limits<unsigned int>::max());
        }

        static ui_range automatic(const unsigned int min, const unsigned int max)
        {
            return {min, min, max};
        }

        unsigned int current;
        unsigned int min;
        unsigned int max;
    };

    struct ui_padding
    {
        unsigned int before;
        unsigned int after;
    };

    struct ui_axis
    {
        ui_range positioning;
        ui_range sizing;
        ui_padding padding;
        float content_alignment; //0-1 percentage

        //Automatically calculated by layout functions
        unsigned int computed_absolute_position;
        unsigned int computed_content_size_min;
    };

    struct ui_object
    {
        std::array<ui_axis, 3> transform = {};

        unsigned int layout_axis;
        unsigned int content_spacing;

        int sizing_priority = 0; //Higher sizing priority will try and match desired size first
        std::vector<ui_object> content;
    };

    void layout_ui(ui_object& root);
}
