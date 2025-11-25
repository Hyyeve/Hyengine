#include "gl_enums.hpp"

namespace hyengine
{
    namespace texture_formats
    {
        bool is_floating_point(const GLenum format)
        {
            return !is_signed_integer(format) && !is_unsigned_integer(format);
        }

        bool is_signed_integer(const GLenum format)
        {
            switch (format)
            {
                case R8_I:
                case R16_I:
                case R32_I:
                case RG8_I:
                case RG16:
                case RG32_I:
                case RGB8_I:
                case RGB16_I:
                case RGB32_I:
                case RGBA8_I:
                case RGBA16_I:
                case RGBA32_I:
                {
                    return true;
                }

                default: return false;
            }
        }

        bool is_unsigned_integer(const GLenum format)
        {
            switch (format)
            {
                case R8_UI:
                case R16_UI:
                case R32_UI:
                case RG8_UI:
                case RG16:
                case RG32_UI:
                case RGB8_UI:
                case RGB16_UI:
                case RGB32_UI:
                case RGBA8_UI:
                case RGBA16_UI:
                case RGBA32_UI:
                case RGB10_A2_UI:
                {
                    return true;
                }

                default: return false;
            }
        }
    }
}
