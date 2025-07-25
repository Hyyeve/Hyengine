#pragma once

#define DEF_OPERATOR(op) \
    interpolatable operator op(const interpolatable& rhs) const { \
        return interpolatable(current_value op rhs.current_value); \
    } \
    \
    interpolatable operator op(const T& rhs) const { \
        return interpolatable(current_value op rhs); \
    } \
    friend interpolatable operator op(const T& lhs, const interpolatable& rhs) { \
        return interpolatable(lhs op rhs.current_value); \
    }

#define DEF_ASSIGN_OPERATOR(op) \
    interpolatable& operator op(const interpolatable& rhs) { \
        current_value op rhs.current_value; \
        return *this; \
    } \
    \
    interpolatable& operator op(const T& rhs) { \
        current_value op rhs; \
        return *this; \
    }
#include <tracy/Tracy.hpp>

namespace hyengine
{
    template <class T>
    struct interpolator
    {
        template <typename P>
        [[nodiscard]] static T interpolate(const T& a, const T& b, P percent)
        {
            ZoneScoped;
            return a + (b - a) * percent;
        }
    };

    template <>
    struct interpolator<glm::quat>
    {
        template <typename P>
        [[nodiscard]] static glm::quat interpolate(const glm::quat& a, const glm::quat& b, P percent)
        {
            ZoneScoped;
            return glm::slerp(a, b, percent);
        }
    };

    template <class T, class I = interpolator<T>>
    class interpolatable
    {
    public:
        T previous_value;
        T current_value;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        interpolatable(T value) : previous_value(value), current_value(value)
        {
        } // NOLINT(*-explicit-constructor, *-explicit-conversions)

        // ReSharper disable once CppNonExplicitConversionOperator
        operator T()
        { // NOLINT(*-explicit-constructor, *-explicit-conversions)
            return current_value;
        }

        interpolatable& operator=(const T& rhs)
        {
            current_value = rhs;
            return *this;
        }

        DEF_OPERATOR(+)
        DEF_OPERATOR(-)
        DEF_OPERATOR(*)
        DEF_OPERATOR(/)

        DEF_OPERATOR(%)
        DEF_OPERATOR(&)
        DEF_OPERATOR(|)
        DEF_OPERATOR(^)
        DEF_OPERATOR(<<)
        DEF_OPERATOR(>>)

        DEF_ASSIGN_OPERATOR(+=)
        DEF_ASSIGN_OPERATOR(-=)
        DEF_ASSIGN_OPERATOR(*=)
        DEF_ASSIGN_OPERATOR(/=)

        DEF_ASSIGN_OPERATOR(%=)
        DEF_ASSIGN_OPERATOR(&=)
        DEF_ASSIGN_OPERATOR(|=)
        DEF_ASSIGN_OPERATOR(^=)

        DEF_ASSIGN_OPERATOR(<<=)
        DEF_ASSIGN_OPERATOR(>>=)

        template <typename P>
        [[nodiscard]] T interpolated(const P percent) const
        {
            ZoneScoped;
            return I::interpolate(previous_value, current_value, percent);
        }

        template <typename P>
        void interpolate_previous_to(const P percent)
        {
            ZoneScoped;
            previous_value = interpolated(percent);
        }

        template <typename P>
        void interpolate_current_to(const P percent)
        {
            ZoneScoped;
            current_value = interpolated(percent);
        }

        template <typename P>
        void stabilize_to(const P percent)
        {
            ZoneScoped;
            current_value = interpolated(percent);
            previous_value = current_value;
        }

        void stabilize()
        {
            ZoneScoped;
            previous_value = current_value;
        }
    };
}

#undef DEF_OPERATOR
#undef DEF_ASSIGN_OPERATOR
