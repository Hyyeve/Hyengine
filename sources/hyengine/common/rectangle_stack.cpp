#include "rectangle_stack.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    void rectangle_stack::push(const aa_rectangle& rect, const rectangle_stack::stack_mode mode)
    {
        ZoneScoped;
        aa_rectangle scissor = rect;
        if (!stack.empty() && mode != rectangle_stack::stack_mode::OVERWRITE)
        {
            const aa_rectangle& prev = stack.top();
            switch (mode)
            {
                case rectangle_stack::stack_mode::UNION: scissor = prev.union_with(rect);
                    break;
                default: scissor = prev.intersection_with(rect);
                    break;
            }
        }

        stack.push(scissor);
    }

    void rectangle_stack::swap(const aa_rectangle& rect, const rectangle_stack::stack_mode mode)
    {
        pop();
        push(rect, mode);
    }

    void rectangle_stack::pop()
    {
        if (!stack.empty()) stack.pop();
    }

    const aa_rectangle& rectangle_stack::peek()
    {
        return stack.top();
    }
}
