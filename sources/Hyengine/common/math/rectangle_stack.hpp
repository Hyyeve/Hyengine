#pragma once
#include <stack>
#include "rectangle.hpp"

namespace hyengine::common
{
    class rectangle_stack {
    public:
        enum class stack_mode {
            OVERWRITE, UNION, INTERSECTION
        };

        void push(const common::rectangle& rect, stack_mode mode);
        void swap(const common::rectangle& rect, stack_mode mode);
        void pop();

        [[nodiscard]] const common::rectangle& peek();
    private:
        std::stack<rectangle> stack;
    };
}


