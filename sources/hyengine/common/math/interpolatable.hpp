#pragma once

#define DEF_OPERATOR(op) \
    interpolatable operator op(const interpolatable& rhs) const { \
        return interpolatable(current_value op rhs.current_value); \
    } \
    \
    interpolatable operator op(const type& rhs) const { \
        return interpolatable(current_value op rhs); \
    } \
    friend interpolatable operator op(const type& lhs, const interpolatable& rhs) { \
        return interpolatable(lhs op rhs.current_value); \
    }

#define DEF_ASSIGN_OPERATOR(op) \
    interpolatable& operator op(const interpolatable& rhs) { \
        current_value op rhs.current_value; \
        return *this; \
    } \
    \
    interpolatable& operator op(const type& rhs) { \
        current_value op rhs; \
        return *this; \
    }

namespace hyengine
{
    template <class type>
    struct interpolator
    {
        template <typename ptype>
        [[nodiscard]] static type interpolate(const type& a, const type& b, ptype percent)
        {
            return a + (b - a) * percent;
        }
    };

    ///Specialized interpolator for quaternions that uses slerp interpolation
    template <>
    struct interpolator<glm::quat>
    {
        template <typename ptype>
        [[nodiscard]] static glm::quat interpolate(const glm::quat& a, const glm::quat& b, ptype percent)
        {
            return glm::slerp(a, b, percent);
        }
    };

    ///Template for wrapping values to allow easily storing a previous/current value and interpolating.
    template <class type, class interpolator = interpolator<type>>
    class interpolatable
    {
    public:
        type previous_value;
        type current_value;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        interpolatable(type value) : previous_value(value), current_value(value) // NOLINT(*-explicit-constructor)
        {}

        // ReSharper disable once CppNonExplicitConversionOperator
        operator type() // NOLINT(*-explicit-constructor)
        {
            return current_value;
        }

        interpolatable& operator=(const type& rhs)
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

        ///Returns an interpolated value between the previous and current values
        template <typename ptype>
        [[nodiscard]] type interpolated(const ptype percent) const
        {
            return interpolator::interpolate(previous_value, current_value, percent);
        }

        ///Updates the previous value based on an interpolation percent between current and previous values
        template <typename ptype>
        void interpolate_previous_to(const ptype percent)
        {
            previous_value = interpolated(percent);
        }

        ///Updates the current value based on an interpolation percent between current and previous values
        template <typename ptype>
        void interpolate_current_to(const ptype percent)
        {
            current_value = interpolated(percent);
        }

        ///Resets the previous value to the current value (meaning all interpolation will return the current value)
        void reset()
        {
            previous_value = current_value;
        }
    };
}

#undef DEF_OPERATOR
#undef DEF_ASSIGN_OPERATOR
