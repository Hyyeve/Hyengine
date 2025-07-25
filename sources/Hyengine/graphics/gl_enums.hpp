#pragma once
#include "glad/glad.h"

namespace hyengine
{
    namespace cull_modes
    {
    }

    namespace buffer_targets
    {
        inline GLenum ARRAY = GL_ARRAY_BUFFER;
        inline GLenum ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER;
        inline GLenum COPY_READ = GL_COPY_READ_BUFFER;
        inline GLenum COPY_WRITE = GL_COPY_WRITE_BUFFER;
        inline GLenum DISPATCH_INDIRECT = GL_DISPATCH_INDIRECT_BUFFER;
        inline GLenum DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER;
        inline GLenum ELEMENT = GL_ELEMENT_ARRAY_BUFFER;
        inline GLenum PIXEL_PACK = GL_PIXEL_PACK_BUFFER;
        inline GLenum PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER;
        inline GLenum QUERY = GL_QUERY_BUFFER;
        inline GLenum SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER;
        inline GLenum TEXTURE = GL_TEXTURE_BUFFER;
        inline GLenum TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER;
        inline GLenum UNIFORM = GL_UNIFORM_BUFFER;
    }

    namespace framebuffer_targets
    {
        inline GLenum DRAW = GL_DRAW_FRAMEBUFFER;
        inline GLenum READ = GL_READ_FRAMEBUFFER;
        inline GLenum BOTH = GL_FRAMEBUFFER;
    }

    namespace texture_formats
    {
        inline GLenum R8 = GL_R8;
        inline GLenum R8_SNORM = GL_R8_SNORM;
        inline GLenum R16 = GL_R16;
        inline GLenum R16_SNORM = GL_R16_SNORM;
        inline GLenum RG8 = GL_RG8;
        inline GLenum RG8_SNORM = GL_RG8_SNORM;
        inline GLenum RG16 = GL_RG16;
        inline GLenum RG16_SNORM = GL_RG16_SNORM;
        inline GLenum R3_G3_B2 = GL_R3_G3_B2;
        inline GLenum RGB4 = GL_RGB4;
        inline GLenum RGB5 = GL_RGB5;
        inline GLenum RGB8 = GL_RGB8;
        inline GLenum RGB8_SNORM = GL_RGB8_SNORM;
        inline GLenum RGB10 = GL_RGB10;
        inline GLenum RGB12 = GL_RGB12;
        inline GLenum RGB16_SNORM = GL_RGB16_SNORM;
        inline GLenum RGBA2 = GL_RGBA2;
        inline GLenum RGBA4 = GL_RGBA4;
        inline GLenum RGB5_A1 = GL_RGB5_A1;
        inline GLenum RGBA8 = GL_RGBA8;
        inline GLenum RGBA8_SNORM = GL_RGBA8_SNORM;
        inline GLenum RGB10_A2 = GL_RGB10_A2;
        inline GLenum RGB10_A2UI = GL_RGB10_A2UI;
        inline GLenum RGBA12 = GL_RGBA12;
        inline GLenum RGBA16 = GL_RGBA16;
        inline GLenum SRGB8 = GL_SRGB8;
        inline GLenum SRGB8_ALPHA8 = GL_SRGB8_ALPHA8;
        inline GLenum R16F = GL_R16F;
        inline GLenum RG16F = GL_RG16F;
        inline GLenum RGB16F = GL_RGB16F;
        inline GLenum RGBA16F = GL_RGBA16F;
        inline GLenum R32F = GL_R32F;
        inline GLenum RG32F = GL_RG32F;
        inline GLenum RGB32F = GL_RGB32F;
        inline GLenum RGBA32F = GL_RGBA32F;
        inline GLenum R11F_G11F_B10F = GL_R11F_G11F_B10F;
        inline GLenum RGB9_E5 = GL_RGB9_E5;
        inline GLenum R8I = GL_R8I;
        inline GLenum R8UI = GL_R8UI;
        inline GLenum R16I = GL_R16I;
        inline GLenum R16UI = GL_R16UI;
        inline GLenum R32I = GL_R32I;
        inline GLenum R32UI = GL_R32UI;
        inline GLenum RG8I = GL_RG8I;
        inline GLenum RG8UI = GL_RG8UI;
        inline GLenum RG16I = GL_RG16I;
        inline GLenum RG16UI = GL_RG16UI;
        inline GLenum RG32I = GL_RG32I;
        inline GLenum RG32UI = GL_RG32UI;
        inline GLenum RGB8I = GL_RGB8I;
        inline GLenum RGB8UI = GL_RGB8UI;
        inline GLenum RGB16I = GL_RGB16I;
        inline GLenum RGB16UI = GL_RGB16UI;
        inline GLenum RGB32I = GL_RGB32I;
        inline GLenum RGB32UI = GL_RGB32UI;
        inline GLenum RGBA8I = GL_RGBA8I;
        inline GLenum RGBA8UI = GL_RGBA8UI;
        inline GLenum RGBA16I = GL_RGBA16I;
        inline GLenum RGBA16UI = GL_RGBA16UI;
        inline GLenum RGBA32I = GL_RGBA32I;
        inline GLenum RGBA32UI = GL_RGBA32UI;
    }

    namespace blend_coefficients
    {
        inline GLenum ZERO = GL_ZERO;
        inline GLenum ONE = GL_ONE;
        inline GLenum SRC_COL = GL_SRC_COLOR;
        inline GLenum INVERSE_SRC_COL = GL_ONE_MINUS_SRC_COLOR;
        inline GLenum DST_COL = GL_DST_COLOR;
        inline GLenum INVERSE_DST_COL = GL_ONE_MINUS_DST_COLOR;
        inline GLenum SRC_ALPHA = GL_SRC_ALPHA;
        inline GLenum INVERSE_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
        inline GLenum DST_ALPHA = GL_DST_ALPHA;
        inline GLenum INVERSE_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;
        inline GLenum CONST_COL = GL_CONSTANT_COLOR;
        inline GLenum INVERSE_CONST_COL = GL_ONE_MINUS_CONSTANT_COLOR;
        inline GLenum CONST_ALPHA = GL_CONSTANT_ALPHA;
        inline GLenum INVERSE_CONST_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA;
        inline GLenum SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE;
        inline GLenum SRC1_COLOR = GL_SRC1_COLOR;
        inline GLenum INVERSE_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR;
        inline GLenum SRC1_ALPHA = GL_SRC1_ALPHA;
        inline GLenum INVERSE_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA;
    }

    namespace blend_functions
    {
        inline GLenum ADD = GL_FUNC_ADD;
        inline GLenum SUBTRACT = GL_FUNC_SUBTRACT;
        inline GLenum SUBTRACT_SWAPPED = GL_FUNC_REVERSE_SUBTRACT;
        inline GLenum MIN = GL_MIN;
        inline GLenum MAX = GL_MAX;
    }
}
