#pragma once
#include <array>
#include <limits>
#include <vector>


namespace hyui {

    struct range_value
    {
        static range_value fixed(const unsigned int value)
        {
            return { value, value, value};
        }

        static range_value automatic()
        {
            return automatic(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
        }

        static range_value automatic(const unsigned int min)
        {
            return automatic(min, std::numeric_limits<unsigned int>::max());
        }

        static range_value automatic(const unsigned int min, const unsigned int max)
        {
            return { min, min, max };
        }

        unsigned int current;
        unsigned int min;
        unsigned int max;
    };

    struct axis_padding
    {
        unsigned int start;
        unsigned int end;
    };

    struct axis_transform
    {
        range_value positioning;
        range_value sizing;
        axis_padding padding;
        float content_alignment;

        unsigned int absolute_position;
        unsigned int content_size_min;
    };

    struct axis_layout
    {
        unsigned int transform_axis;
        unsigned int content_spacing;
    };

    struct ui_object
    {
        std::array<axis_transform, 3> transform = {};
        axis_layout layout = {0, 0};
        int sizing_priority = 0;
        std::vector<ui_object> content;
    };

    void layout_tree(ui_object& root);
}
