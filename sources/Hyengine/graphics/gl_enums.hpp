#pragma once
#include "glad/glad.h"

namespace hyengine
{
    namespace cull_modes
    {
    }

    namespace buffer_targets
    {
        constexpr inline GLenum ARRAY = GL_ARRAY_BUFFER;
        constexpr inline GLenum ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER;
        constexpr inline GLenum COPY_READ = GL_COPY_READ_BUFFER;
        constexpr inline GLenum COPY_WRITE = GL_COPY_WRITE_BUFFER;
        constexpr inline GLenum DISPATCH_INDIRECT = GL_DISPATCH_INDIRECT_BUFFER;
        constexpr inline GLenum DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER;
        constexpr inline GLenum ELEMENT = GL_ELEMENT_ARRAY_BUFFER;
        constexpr inline GLenum PIXEL_PACK = GL_PIXEL_PACK_BUFFER;
        constexpr inline GLenum PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER;
        constexpr inline GLenum QUERY = GL_QUERY_BUFFER;
        constexpr inline GLenum SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER;
        constexpr inline GLenum TEXTURE = GL_TEXTURE_BUFFER;
        constexpr inline GLenum TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER;
        constexpr inline GLenum UNIFORM = GL_UNIFORM_BUFFER;
    }

    namespace framebuffer_targets
    {
        constexpr inline GLenum DRAW = GL_DRAW_FRAMEBUFFER;
        constexpr inline GLenum READ = GL_READ_FRAMEBUFFER;
        constexpr inline GLenum BOTH = GL_FRAMEBUFFER;
    }

    namespace texture_formats
    {
        //NOTE: Does not validate that it *is* a floating point format, only that it is NOT a standard integer format
        bool is_floating_point(const GLenum format);
        bool is_signed_integer(const GLenum format);
        bool is_unsigned_integer(const GLenum format);

        constexpr inline GLenum R8 = GL_R8;
        constexpr inline GLenum R8_SNORM = GL_R8_SNORM;
        constexpr inline GLenum R16 = GL_R16;
        constexpr inline GLenum R16_SNORM = GL_R16_SNORM;
        constexpr inline GLenum RG8 = GL_RG8;
        constexpr inline GLenum RG8_SNORM = GL_RG8_SNORM;
        constexpr inline GLenum RG16 = GL_RG16;
        constexpr inline GLenum RG16_SNORM = GL_RG16_SNORM;
        constexpr inline GLenum R3_G3_B2 = GL_R3_G3_B2;
        constexpr inline GLenum RGB4 = GL_RGB4;
        constexpr inline GLenum RGB5 = GL_RGB5;
        constexpr inline GLenum RGB8 = GL_RGB8;
        constexpr inline GLenum RGB8_SNORM = GL_RGB8_SNORM;
        constexpr inline GLenum RGB10 = GL_RGB10;
        constexpr inline GLenum RGB12 = GL_RGB12;
        constexpr inline GLenum RGB16_SNORM = GL_RGB16_SNORM;
        constexpr inline GLenum RGBA2 = GL_RGBA2;
        constexpr inline GLenum RGBA4 = GL_RGBA4;
        constexpr inline GLenum RGB5_A1 = GL_RGB5_A1;
        constexpr inline GLenum RGBA8 = GL_RGBA8;
        constexpr inline GLenum RGBA8_SNORM = GL_RGBA8_SNORM;
        constexpr inline GLenum RGB10_A2 = GL_RGB10_A2;
        constexpr inline GLenum RGB10_A2UI = GL_RGB10_A2UI;
        constexpr inline GLenum RGBA12 = GL_RGBA12;
        constexpr inline GLenum RGBA16 = GL_RGBA16;
        constexpr inline GLenum SRGB8 = GL_SRGB8;
        constexpr inline GLenum SRGB8_ALPHA8 = GL_SRGB8_ALPHA8;
        constexpr inline GLenum R16F = GL_R16F;
        constexpr inline GLenum RG16F = GL_RG16F;
        constexpr inline GLenum RGB16F = GL_RGB16F;
        constexpr inline GLenum RGBA16F = GL_RGBA16F;
        constexpr inline GLenum R32F = GL_R32F;
        constexpr inline GLenum RG32F = GL_RG32F;
        constexpr inline GLenum RGB32F = GL_RGB32F;
        constexpr inline GLenum RGBA32F = GL_RGBA32F;
        constexpr inline GLenum R11F_G11F_B10F = GL_R11F_G11F_B10F;
        constexpr inline GLenum RGB9_E5 = GL_RGB9_E5;
        constexpr inline GLenum R8I = GL_R8I;
        constexpr inline GLenum R8UI = GL_R8UI;
        constexpr inline GLenum R16I = GL_R16I;
        constexpr inline GLenum R16UI = GL_R16UI;
        constexpr inline GLenum R32I = GL_R32I;
        constexpr inline GLenum R32UI = GL_R32UI;
        constexpr inline GLenum RG8I = GL_RG8I;
        constexpr inline GLenum RG8UI = GL_RG8UI;
        constexpr inline GLenum RG16I = GL_RG16I;
        constexpr inline GLenum RG16UI = GL_RG16UI;
        constexpr inline GLenum RG32I = GL_RG32I;
        constexpr inline GLenum RG32UI = GL_RG32UI;
        constexpr inline GLenum RGB8I = GL_RGB8I;
        constexpr inline GLenum RGB8UI = GL_RGB8UI;
        constexpr inline GLenum RGB16I = GL_RGB16I;
        constexpr inline GLenum RGB16UI = GL_RGB16UI;
        constexpr inline GLenum RGB32I = GL_RGB32I;
        constexpr inline GLenum RGB32UI = GL_RGB32UI;
        constexpr inline GLenum RGBA8I = GL_RGBA8I;
        constexpr inline GLenum RGBA8UI = GL_RGBA8UI;
        constexpr inline GLenum RGBA16I = GL_RGBA16I;
        constexpr inline GLenum RGBA16UI = GL_RGBA16UI;
        constexpr inline GLenum RGBA32I = GL_RGBA32I;
        constexpr inline GLenum RGBA32UI = GL_RGBA32UI;
    }

    namespace blend_coefficients
    {
        constexpr inline GLenum ZERO = GL_ZERO;
        constexpr inline GLenum ONE = GL_ONE;
        constexpr inline GLenum SRC_COL = GL_SRC_COLOR;
        constexpr inline GLenum INVERSE_SRC_COL = GL_ONE_MINUS_SRC_COLOR;
        constexpr inline GLenum DST_COL = GL_DST_COLOR;
        constexpr inline GLenum INVERSE_DST_COL = GL_ONE_MINUS_DST_COLOR;
        constexpr inline GLenum SRC_ALPHA = GL_SRC_ALPHA;
        constexpr inline GLenum INVERSE_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
        constexpr inline GLenum DST_ALPHA = GL_DST_ALPHA;
        constexpr inline GLenum INVERSE_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;
        constexpr inline GLenum CONST_COL = GL_CONSTANT_COLOR;
        constexpr inline GLenum INVERSE_CONST_COL = GL_ONE_MINUS_CONSTANT_COLOR;
        constexpr inline GLenum CONST_ALPHA = GL_CONSTANT_ALPHA;
        constexpr inline GLenum INVERSE_CONST_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA;
        constexpr inline GLenum SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE;
        constexpr inline GLenum SRC1_COLOR = GL_SRC1_COLOR;
        constexpr inline GLenum INVERSE_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR;
        constexpr inline GLenum SRC1_ALPHA = GL_SRC1_ALPHA;
        constexpr inline GLenum INVERSE_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA;
    }

    namespace blend_functions
    {
        constexpr inline GLenum ADD = GL_FUNC_ADD;
        constexpr inline GLenum SUBTRACT = GL_FUNC_SUBTRACT;
        constexpr inline GLenum SUBTRACT_SWAPPED = GL_FUNC_REVERSE_SUBTRACT;
        constexpr inline GLenum MIN = GL_MIN;
        constexpr inline GLenum MAX = GL_MAX;
    }
}
