#pragma once
#include "glad/glad.h"

namespace hyengine
{
    #define DECL constexpr inline GLenum
    #define DECLA(n) DECL n = GL_##n

    namespace gl_i32_consts
    {
        DECLA(MAJOR_VERSION);
        DECLA(MINOR_VERSION);
        DECLA(MAX_COMPUTE_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_COMBINED_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_COMPUTE_UNIFORM_BLOCKS);
        DECLA(MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
        DECLA(MAX_COMPUTE_UNIFORM_COMPONENTS);
        DECLA(MAX_COMPUTE_ATOMIC_COUNTERS);
        DECLA(MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
        DECLA(MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS);
        DECLA(MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
        DECLA(MAX_DEBUG_GROUP_STACK_DEPTH);
        DECLA(MAX_3D_TEXTURE_SIZE);
        DECLA(MAX_ARRAY_TEXTURE_LAYERS);
        DECLA(MAX_CLIP_DISTANCES);
        DECLA(MAX_COLOR_TEXTURE_SAMPLES);
        DECLA(MAX_COMBINED_ATOMIC_COUNTERS);
        DECLA(MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
        DECLA(MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
        DECLA(MAX_COMBINED_TEXTURE_IMAGE_UNITS);
        DECLA(MAX_COMBINED_UNIFORM_BLOCKS);
        DECLA(MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
        DECLA(MAX_CUBE_MAP_TEXTURE_SIZE);
        DECLA(MAX_DEPTH_TEXTURE_SAMPLES);
        DECLA(MAX_DRAW_BUFFERS);
        DECLA(MAX_DUAL_SOURCE_DRAW_BUFFERS);
        DECLA(MAX_ELEMENTS_INDICES);
        DECLA(MAX_ELEMENTS_VERTICES);
        DECLA(MAX_FRAGMENT_ATOMIC_COUNTERS);
        DECLA(MAX_FRAGMENT_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_FRAGMENT_INPUT_COMPONENTS);
        DECLA(MAX_FRAGMENT_UNIFORM_COMPONENTS);
        DECLA(MAX_FRAGMENT_UNIFORM_BLOCKS);
        DECLA(MAX_FRAMEBUFFER_WIDTH);
        DECLA(MAX_FRAMEBUFFER_HEIGHT);
        DECLA(MAX_FRAMEBUFFER_LAYERS);
        DECLA(MAX_FRAMEBUFFER_SAMPLES);
        DECLA(MAX_GEOMETRY_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_GEOMETRY_INPUT_COMPONENTS);
        DECLA(MAX_GEOMETRY_OUTPUT_COMPONENTS);
        DECLA(MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
        DECLA(MAX_GEOMETRY_UNIFORM_BLOCKS);
        DECLA(MAX_GEOMETRY_UNIFORM_COMPONENTS);
        DECLA(MAX_INTEGER_SAMPLES);
        DECLA(MAX_LABEL_LENGTH);
        DECLA(MIN_PROGRAM_TEXEL_OFFSET);
        DECLA(MAX_RECTANGLE_TEXTURE_SIZE);
        DECLA(MAX_RENDERBUFFER_SIZE);
        DECLA(MAX_SAMPLE_MASK_WORDS);
        DECLA(MAX_SERVER_WAIT_TIMEOUT);
        DECLA(MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        DECLA(MAX_TESS_CONTROL_ATOMIC_COUNTERS);
        DECLA(MAX_TESS_EVALUATION_ATOMIC_COUNTERS);
        DECLA(MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_TEXTURE_BUFFER_SIZE);
        DECLA(MAX_TEXTURE_IMAGE_UNITS);
        DECLA(MAX_TEXTURE_LOD_BIAS);
        DECLA(MAX_TEXTURE_SIZE);
        DECLA(MAX_UNIFORM_BUFFER_BINDINGS);
        DECLA(MAX_UNIFORM_BLOCK_SIZE);
        DECLA(MAX_UNIFORM_LOCATIONS);
        DECLA(MAX_VARYING_COMPONENTS);
        DECLA(MAX_VARYING_VECTORS);
        DECLA(MAX_VARYING_FLOATS);
        DECLA(MAX_VERTEX_ATOMIC_COUNTERS);
        DECLA(MAX_VERTEX_ATTRIBS);
        DECLA(MAX_VERTEX_SHADER_STORAGE_BLOCKS);
        DECLA(MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        DECLA(MAX_VERTEX_UNIFORM_COMPONENTS);
        DECLA(MAX_VERTEX_OUTPUT_COMPONENTS);
        DECLA(MAX_VERTEX_UNIFORM_BLOCKS);
        DECLA(MAX_VIEWPORTS);
        DECL STEREO_RENDERING = GL_STEREO;
        DECLA(SUBPIXEL_BITS);
        DECLA(MAX_VERTEX_ATTRIB_RELATIVE_OFFSET);
        DECLA(MAX_VERTEX_ATTRIB_BINDINGS);
        DECLA(VIEWPORT_SUBPIXEL_BITS);
        DECLA(MAX_ELEMENT_INDEX);
    }

    namespace gl_flags
    {
        DECL BLENDING = GL_BLEND;
        DECL COLOR_LOGIC_OP = GL_COLOR_LOGIC_OP;
        DECL FACE_CULLING = GL_CULL_FACE;
        DECL DEBUG_OUTPUT = GL_DEBUG_OUTPUT;
        DECL DEBUG_OUTPUT_SYNCHRONOUS = GL_DEBUG_OUTPUT_SYNCHRONOUS;
        DECL DEPTH_CLAMP = GL_DEPTH_CLAMP;
        DECL DEPTH_TEST = GL_DEPTH_TEST;
        DECL DITHER = GL_DITHER;
        DECL FRAMEBUFFER_SRGB = GL_FRAMEBUFFER_SRGB;
        DECL LINE_SMOOTH = GL_LINE_SMOOTH;
        DECL MULTISAMPLE = GL_MULTISAMPLE;
        DECL POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL;
        DECL POLYGON_OFFSET_LINE = GL_POLYGON_OFFSET_LINE;
        DECL POLYGON_OFFSET_POINT = GL_POLYGON_OFFSET_POINT;
        DECL POLYGON_SMOOTH = GL_POLYGON_SMOOTH;
        DECL PRIMITIVE_RESTART = GL_PRIMITIVE_RESTART;
        DECL PRIMITIVE_RESTART_FIXED_INDEX = GL_PRIMITIVE_RESTART_FIXED_INDEX;
        DECL RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD;
        DECL ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE;
        DECL ALPHA_TO_ONE = GL_SAMPLE_ALPHA_TO_ONE;
        DECL SAMPLE_VERAGE = GL_SAMPLE_COVERAGE;
        DECL SAMPLE_SHADING = GL_SAMPLE_SHADING;
        DECL SAMPLE_MASK = GL_SAMPLE_MASK;
        DECL SCISSOR_TEST = GL_SCISSOR_TEST;
        DECL STENCIL_TEST = GL_STENCIL_TEST;
        DECL SEAMLESS_CUBE_MAP = GL_TEXTURE_CUBE_MAP_SEAMLESS;
        DECL PROGRAM_POINT_SIZE = GL_PROGRAM_POINT_SIZE;
    }

    namespace cull_modes
    {
        DECL FRONT = GL_FRONT;
        DECL BACK = GL_BACK;
        DECL ALL = GL_FRONT_AND_BACK;
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
        ///NOTE: Does not validate that it *is* a floating point format, only that it is NOT a standard integer format
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
        constexpr inline GLenum RGB10_A2_UI = GL_RGB10_A2UI;
        constexpr inline GLenum RGBA12 = GL_RGBA12;
        constexpr inline GLenum RGBA16 = GL_RGBA16;
        constexpr inline GLenum SRGB8 = GL_SRGB8;
        constexpr inline GLenum SRGB8_ALPHA8 = GL_SRGB8_ALPHA8;
        constexpr inline GLenum R16_F = GL_R16F;
        constexpr inline GLenum RG16_F = GL_RG16F;
        constexpr inline GLenum RGB16_F = GL_RGB16F;
        constexpr inline GLenum RGBA16_F = GL_RGBA16F;
        constexpr inline GLenum R32_F = GL_R32F;
        constexpr inline GLenum RG32_F = GL_RG32F;
        constexpr inline GLenum RGB32_F = GL_RGB32F;
        constexpr inline GLenum RGBA32_F = GL_RGBA32F;
        constexpr inline GLenum R11_F_G11_F_B10_F = GL_R11F_G11F_B10F;
        constexpr inline GLenum RGB9_E5 = GL_RGB9_E5;
        constexpr inline GLenum R8_I = GL_R8I;
        constexpr inline GLenum R8_UI = GL_R8UI;
        constexpr inline GLenum R16_I = GL_R16I;
        constexpr inline GLenum R16_UI = GL_R16UI;
        constexpr inline GLenum R32_I = GL_R32I;
        constexpr inline GLenum R32_UI = GL_R32UI;
        constexpr inline GLenum RG8_I = GL_RG8I;
        constexpr inline GLenum RG8_UI = GL_RG8UI;
        constexpr inline GLenum RG16_I = GL_RG16I;
        constexpr inline GLenum RG16_UI = GL_RG16UI;
        constexpr inline GLenum RG32_I = GL_RG32I;
        constexpr inline GLenum RG32_UI = GL_RG32UI;
        constexpr inline GLenum RGB8_I = GL_RGB8I;
        constexpr inline GLenum RGB8_UI = GL_RGB8UI;
        constexpr inline GLenum RGB16_I = GL_RGB16I;
        constexpr inline GLenum RGB16_UI = GL_RGB16UI;
        constexpr inline GLenum RGB32_I = GL_RGB32I;
        constexpr inline GLenum RGB32_UI = GL_RGB32UI;
        constexpr inline GLenum RGBA8_I = GL_RGBA8I;
        constexpr inline GLenum RGBA8_UI = GL_RGBA8UI;
        constexpr inline GLenum RGBA16_I = GL_RGBA16I;
        constexpr inline GLenum RGBA16_UI = GL_RGBA16UI;
        constexpr inline GLenum RGBA32_I = GL_RGBA32I;
        constexpr inline GLenum RGBA32_UI = GL_RGBA32UI;
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

    #undef DECL
    #undef DECLA
}
