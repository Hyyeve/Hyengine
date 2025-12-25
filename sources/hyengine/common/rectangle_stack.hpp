#pragma once
#include <stack>
#include "math/aa_rectangle.hpp"

namespace hyengine
{
    ///Simple push/pop stack of rectangles with union/intersection operations. Useful for keeping track of scissor rectangles or similar stacked masks.
    class rectangle_stack
    {
    public:
        enum class stack_mode
        {
            OVERWRITE, UNION, INTERSECTION
        };

        void push(const aa_rectangle& rect, stack_mode mode);
        void swap(const aa_rectangle& rect, stack_mode mode);
        void pop();

        [[nodiscard]] const aa_rectangle& peek();

    private:
        std::stack<aa_rectangle> stack;
    };
}
