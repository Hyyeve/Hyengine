#include "rectangle_stack.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine::common
{
    void rectangle_stack::push(const rectangle& rect, const rectangle_stack::stack_mode mode) {
        ZoneScoped;
        rectangle scissor = rect;
        if (!stack.empty() && mode != rectangle_stack::stack_mode::OVERWRITE)
        {
            const rectangle& prev = stack.top();
            switch (mode)
            {
                case rectangle_stack::stack_mode::UNION:
                    scissor = prev.union_with(rect);
                    break;
                default:
                    scissor = prev.intersection_with(rect);
                    break;
            }
        }

        stack.push(scissor);
    }

    void rectangle_stack::swap(const rectangle& rect, const rectangle_stack::stack_mode mode)
    {
        ZoneScoped;
        pop();
        push(rect, mode);
    }

    void rectangle_stack::pop()
    {
        ZoneScoped;
        if (!stack.empty()) stack.pop();
    }

    const rectangle& rectangle_stack::peek()
    {
        ZoneScoped;
        return stack.top();
    }
}
