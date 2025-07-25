#pragma once
#include <stack>
#include "rectangle.hpp"

namespace hyengine
{
    class rectangle_stack
    {
    public:
        enum class stack_mode
        {
            OVERWRITE, UNION, INTERSECTION
        };

        void push(const rectangle& rect, stack_mode mode);
        void swap(const rectangle& rect, stack_mode mode);
        void pop();

        [[nodiscard]] const rectangle& peek();

    private:
        std::stack<rectangle> stack;
    };
}
