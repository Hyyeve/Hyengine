#include "ui_layout.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    u32 content_adjusted_min(const ui_object& object, const i32 axis)
    {
        return std::max(object.transform[axis].sizing.min, object.transform[axis].computed_content_size_min);
    }

    void constrain_size(ui_object& object, const i32 axis)
    {
        const u32 min = content_adjusted_min(object, axis);
        const u32 max = object.transform[axis].sizing.max;
        const u32 current = object.transform[axis].sizing.current;
        object.transform[axis].sizing.current = std::min(std::max(current, min), max);
    }

    void constrain_offset(ui_object& object, const i32 axis)
    {
        const u32 min = object.transform[axis].positioning.min;
        const u32 max = object.transform[axis].positioning.max;
        const u32 current = object.transform[axis].positioning.current;
        object.transform[axis].positioning.current = std::min(std::max(current, min), max);
    }

    bool is_fixed_size(const ui_object& object, const i32 axis)
    {
        const u32 min = content_adjusted_min(object, axis);
        const u32 max = object.transform[axis].sizing.max;
        return min >= max;
    }

    bool can_shrink(const ui_object& object, const i32 axis)
    {
        const u32 min = content_adjusted_min(object, axis);
        const u32 current = object.transform[axis].sizing.current;
        return current > min;
    }

    bool can_expand(const ui_object& object, const i32 axis)
    {
        const u32 max = object.transform[axis].sizing.max;
        const u32 current = object.transform[axis].sizing.current;
        return current < max;
    }

    bool can_resize(const ui_object& object, const i32 axis, const i32 direction)
    {
        return direction > 0 ? can_expand(object, axis) : can_shrink(object, axis);
    }

    i32 constrained_resize_amount(const ui_object& object, const i32 axis, const i32 amount)
    {
        if (amount == 0 || is_fixed_size(object, axis)) return 0;
        const u32 min = content_adjusted_min(object, axis);
        const u32 max = object.transform[axis].sizing.max;
        const u32 current = object.transform[axis].sizing.current;

        i32 actual_amount = 0;

        if (amount > 0)
        {
            if (max - current > amount) actual_amount = amount;
            else actual_amount = max - current;

            return std::max(actual_amount, 0);
        }

        if (current - min > -amount) actual_amount = amount;
        else actual_amount = -(current - min);

        return std::min(actual_amount, 0);
    }

    void fit_to_content(ui_object& container)
    {
        ZoneScoped;
        const std::vector<ui_object>& content = container.content;

        if (content.empty())
        {
            for (ui_axis& axis : container.transform)
            {
                axis.sizing.current = axis.sizing.min;
                axis.computed_content_size_min = 0;
            }
            return;
        }

        for (i32 axis = 0; axis < container.transform.size(); axis++)
        {
            if (is_fixed_size(container, axis)) continue;

            const u32 axis_padding = container.transform[axis].padding.before + container.transform[axis].padding.after;
            const u32 spacing_sum = container.content_spacing * (content.size() - 1);
            const bool layout_axis = container.layout_axis == axis;

            u32 content_total = layout_axis ? spacing_sum : 0;
            u32 content_total_min = layout_axis ? spacing_sum : 0;

            for (ui_object object : content)
            {
                const u32 content_size = object.transform[axis].sizing.current;
                const u32 content_size_min = object.transform[axis].sizing.current;

                if (layout_axis)
                {
                    content_total += content_size;
                    content_total_min += content_size_min;
                }
                else
                {
                    if (content_size > content_total) content_total = content_size;
                    if (content_size_min > content_total_min) content_total_min = content_size_min;
                }
            }

            container.transform[axis].sizing.current = axis_padding + content_total;
            container.transform[axis].computed_content_size_min = axis_padding + content_total_min;

            constrain_size(container, axis);
        }
    }

    void fit_to_container(ui_object& container)
    {
        ZoneScoped;
        std::vector<ui_object>& content = container.content;

        if (content.empty()) return;

        for (i32 axis = 0; axis < container.transform.size(); axis++)
        {
            const u32 container_size = container.transform[axis].sizing.current;
            const u32 axis_padding = container.transform[axis].padding.before + container.transform[axis].padding.after;
            const u32 spacing_sum = container.content_spacing * (content.size() - 1);
            const u32 container_space = container_size - axis_padding - spacing_sum;
            const bool layout_axis = container.layout_axis == axis;

            if (!layout_axis)
            {
                for (ui_object& object : content)
                {
                    object.transform[axis].sizing.current = container_size - axis_padding;
                    constrain_size(object, axis);
                }
                continue;
            }

            u32 content_size = 0;
            for (ui_object& object : content)
            {
                content_size += object.transform[axis].sizing.current;
            }

            i32 remaining_space = container_space - content_size;
            i32 current_priority = std::numeric_limits<i32>::max();

            while (remaining_space != 0)
            {
                i32 next_highest_priority = std::numeric_limits<i32>::min();
                i32 resizable_at_priority = 0;
                for (ui_object& object : content)
                {
                    if (object.sizing_priority < current_priority && object.sizing_priority > next_highest_priority) next_highest_priority = object.sizing_priority;
                    if (object.sizing_priority == current_priority && can_resize(object, axis, remaining_space)) resizable_at_priority++;
                }

                if (resizable_at_priority == 0)
                {
                    if (next_highest_priority == current_priority) break;
                    current_priority = next_highest_priority;
                    continue;
                }

                i32 resize_amount = remaining_space / resizable_at_priority;
                if (resize_amount == 0) resize_amount = 1;

                for (ui_object& object : content)
                {
                    if (!can_resize(object, axis, remaining_space) || object.sizing_priority != current_priority) continue;
                    const i32 actual_resize = constrained_resize_amount(object, axis, resize_amount);
                    object.transform[axis].sizing.current += actual_resize;
                    remaining_space -= actual_resize;
                    if (remaining_space == 0) break;
                }

                current_priority = next_highest_priority;
            }
        }
    }

    void position_elements(ui_object& container)
    {
        ZoneScoped;
        std::vector<ui_object>& content = container.content;
        if (content.empty()) return;

        for (i32 axis = 0; axis < container.transform.size(); axis++)
        {
            const bool layout_axis = container.layout_axis == axis;
            const u32 container_size = container.transform[axis].sizing.current;
            const u32 axis_padding = container.transform[axis].padding.before + container.transform[axis].padding.after;
            const u32 spacing_sum = container.content_spacing * (content.size() - 1);
            const u32 container_space = container_size - axis_padding - (layout_axis ? spacing_sum : 0);

            if (!layout_axis)
            {
                for (ui_object& object : content)
                {
                    const u32 available_space = container_space < object.transform[axis].sizing.current ? 0 : container_space - object.transform[axis].sizing.current;
                    const u32 offset = available_space * container.transform[axis].content_alignment + container.transform[axis].padding.before;
                    object.transform[axis].positioning.current = offset;
                    constrain_offset(object, axis);
                    object.transform[axis].computed_absolute_position = container.transform[axis].computed_absolute_position + object.transform[axis].positioning.current;
                }
                continue;
            }

            u32 content_size = 0;
            for (const ui_object& object : content)
            {
                content_size += object.transform[axis].sizing.current;
            }

            const u32 available_space = container_space < content_size ? 0 : container_space - content_size;

            u32 content_offset = available_space * container.transform[axis].content_alignment + container.transform[axis].padding.before;

            for (ui_object& object : content)
            {
                object.transform[axis].positioning.current = content_offset;
                constrain_offset(object, axis);
                object.transform[axis].computed_absolute_position = container.transform[axis].computed_absolute_position + object.transform[axis].positioning.current;
                content_offset += object.transform[axis].sizing.current + container.content_spacing;
            }
        }
    }

    void fit_tree_containers(ui_object& root)
    {
        for (ui_object& content : root.content)
        {
            fit_tree_containers(content);
        }

        fit_to_content(root);
    }

    void fit_tree_contents(ui_object& root)
    {
        fit_to_container(root);

        for (ui_object& content : root.content)
        {
            fit_tree_contents(content);
        }
    }

    void size_tree(ui_object& root)
    {
        fit_tree_containers(root);
        fit_tree_contents(root);
    }

    void position_tree(ui_object& root)
    {
        for (ui_axis& axis : root.transform) axis.computed_absolute_position = axis.positioning.current;

        position_elements(root);

        for (ui_object& content : root.content)
        {
            position_elements(content);
        }
    }

    void layout_ui(ui_object& root)
    {
        ZoneScoped;
        size_tree(root);
        position_tree(root);
    }
}
