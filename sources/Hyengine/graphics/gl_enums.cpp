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
                case R8I:
                case R16I:
                case R32I:
                case RG8I:
                case RG16:
                case RG32I:
                case RGB8I:
                case RGB16I:
                case RGB32I:
                case RGBA8I:
                case RGBA16I:
                case RGBA32I:
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
                case R8UI:
                case R16UI:
                case R32UI:
                case RG8UI:
                case RG16:
                case RG32UI:
                case RGB8UI:
                case RGB16UI:
                case RGB32UI:
                case RGBA8UI:
                case RGBA16UI:
                case RGBA32UI:
                case RGB10_A2UI:
                {
                    return true;
                }

                default: return false;
            }
        }
    }
}
