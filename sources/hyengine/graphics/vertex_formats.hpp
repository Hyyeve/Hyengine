#pragma once
#include <array>
#include "../library/gl.hpp"

#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    struct vertex_element_type
    {
        GLboolean normalized;
        GLenum gl_type;
        u32 count;
        u32 bytes;
        bool as_int;
    };

    struct vertex_element
    {
        constexpr vertex_element(const vertex_element_type& type, const u32 buffer_binding, const u32 instance_divisor) : type(type), buffer_binding(buffer_binding), instance_divisor(instance_divisor) {}

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr vertex_element(const vertex_element_type& type) : vertex_element(type, 0, 0) {}

        vertex_element_type type;
        u32 buffer_binding;
        u32 instance_divisor;
    };

    template <u32 size>
    class vertex_format
    {
    public:
        constexpr explicit vertex_format(const std::array<vertex_element, size> elements) noexcept : elements(elements)
        {
            total_bytes = 0;
            for (const vertex_element& element : elements)
            {
                total_bytes += element.type.bytes;
            }
        }

        void apply(const GLuint vertex_format_buffer) const
        {
            u32 offset = 0;
            for (u32 i = 0; i < elements.size(); i++)
            {
                const vertex_element& element = elements[i];
                const vertex_element_type& type = element.type;
                glEnableVertexArrayAttrib(vertex_format_buffer, i);

                if (type.as_int)
                {
                    glVertexArrayAttribIFormat(vertex_format_buffer, i, type.count, type.gl_type, offset);
                }
                else if (type.gl_type == GL_DOUBLE)
                {
                    glVertexArrayAttribLFormat(vertex_format_buffer, i, type.count, type.gl_type, offset);
                }
                else
                {
                    glVertexArrayAttribFormat(vertex_format_buffer, i, type.count, type.gl_type, type.normalized, offset);
                }

                glVertexArrayAttribBinding(vertex_format_buffer, i, element.buffer_binding);

                if (element.instance_divisor > 0)
                {
                    glVertexArrayBindingDivisor(vertex_format_buffer, i, element.instance_divisor);
                }

                offset += type.bytes;
            }
        }

    private:
        std::array<vertex_element, size> elements;
        u32 total_bytes = 0;
    };

    namespace vertex_element_types
    {
        constexpr vertex_element_type UBYTE = {GL_FALSE, GL_UNSIGNED_BYTE, 1, 1, true};
        constexpr vertex_element_type UBYTE_UVEC2 = {GL_FALSE, GL_UNSIGNED_BYTE, 2, 2, true};
        constexpr vertex_element_type UBYTE_UVEC3 = {GL_FALSE, GL_UNSIGNED_BYTE, 3, 3, true};
        constexpr vertex_element_type UBYTE_UVEC4 = {GL_FALSE, GL_UNSIGNED_BYTE, 4, 4, true};

        constexpr vertex_element_type UBYTE_FLOAT = {GL_FALSE, GL_UNSIGNED_BYTE, 1, 1, false};
        constexpr vertex_element_type UBYTE_VEC2 = {GL_FALSE, GL_UNSIGNED_BYTE, 2, 2, false};
        constexpr vertex_element_type UBYTE_VEC3 = {GL_FALSE, GL_UNSIGNED_BYTE, 3, 3, false};
        constexpr vertex_element_type UBYTE_VEC4 = {GL_FALSE, GL_UNSIGNED_BYTE, 4, 4, false};

        constexpr vertex_element_type UBYTE_NFLOAT = {GL_TRUE, GL_UNSIGNED_BYTE, 1, 1, false};
        constexpr vertex_element_type UBYTE_NVEC2 = {GL_TRUE, GL_UNSIGNED_BYTE, 2, 2, false};
        constexpr vertex_element_type UBYTE_NVEC3 = {GL_TRUE, GL_UNSIGNED_BYTE, 3, 3, false};
        constexpr vertex_element_type UBYTE_NVEC4 = {GL_TRUE, GL_UNSIGNED_BYTE, 4, 4, false};


        constexpr vertex_element_type USHORT = {GL_FALSE, GL_UNSIGNED_SHORT, 1, 2, true};
        constexpr vertex_element_type USHORT_UVEC2 = {GL_FALSE, GL_UNSIGNED_SHORT, 2, 4, true};
        constexpr vertex_element_type USHORT_UVEC3 = {GL_FALSE, GL_UNSIGNED_SHORT, 3, 6, true};
        constexpr vertex_element_type USHORT_UVEC4 = {GL_FALSE, GL_UNSIGNED_SHORT, 4, 8, true};

        constexpr vertex_element_type USHORT_FLOAT = {GL_FALSE, GL_UNSIGNED_SHORT, 1, 2, false};
        constexpr vertex_element_type USHORT_VEC2 = {GL_FALSE, GL_UNSIGNED_SHORT, 2, 4, false};
        constexpr vertex_element_type USHORT_VEC3 = {GL_FALSE, GL_UNSIGNED_SHORT, 3, 6, false};
        constexpr vertex_element_type USHORT_VEC4 = {GL_FALSE, GL_UNSIGNED_SHORT, 4, 8, false};

        constexpr vertex_element_type USHORT_NFLOAT = {GL_TRUE, GL_UNSIGNED_SHORT, 1, 2, false};
        constexpr vertex_element_type USHORT_NVEC2 = {GL_TRUE, GL_UNSIGNED_SHORT, 2, 4, false};
        constexpr vertex_element_type USHORT_NVEC3 = {GL_TRUE, GL_UNSIGNED_SHORT, 3, 6, false};
        constexpr vertex_element_type USHORT_NVEC4 = {GL_TRUE, GL_UNSIGNED_SHORT, 4, 8, false};


        constexpr vertex_element_type UINT = {GL_FALSE, GL_UNSIGNED_INT, 1, 4, true};
        constexpr vertex_element_type UVEC2 = {GL_FALSE, GL_UNSIGNED_INT, 2, 8, true};
        constexpr vertex_element_type UVEC3 = {GL_FALSE, GL_UNSIGNED_INT, 3, 12, true};
        constexpr vertex_element_type UVEC4 = {GL_FALSE, GL_UNSIGNED_INT, 4, 16, true};

        constexpr vertex_element_type UINT_FLOAT = {GL_FALSE, GL_UNSIGNED_INT, 1, 4, false};
        constexpr vertex_element_type UINT_VEC2 = {GL_FALSE, GL_UNSIGNED_INT, 2, 8, false};
        constexpr vertex_element_type UINT_VEC3 = {GL_FALSE, GL_UNSIGNED_INT, 3, 12, false};
        constexpr vertex_element_type UINT_VEC4 = {GL_FALSE, GL_UNSIGNED_INT, 4, 16, false};

        constexpr vertex_element_type UINT_NFLOAT = {GL_TRUE, GL_UNSIGNED_INT, 1, 4, false};
        constexpr vertex_element_type UINT_NVEC2 = {GL_TRUE, GL_UNSIGNED_INT, 2, 8, false};
        constexpr vertex_element_type UINT_NVEC3 = {GL_TRUE, GL_UNSIGNED_INT, 3, 12, false};
        constexpr vertex_element_type UINT_NVEC4 = {GL_TRUE, GL_UNSIGNED_INT, 4, 16, false};


        constexpr vertex_element_type BYTE = {GL_FALSE, GL_BYTE, 1, 1, true};
        constexpr vertex_element_type BYTE_IVEC2 = {GL_FALSE, GL_BYTE, 2, 2, true};
        constexpr vertex_element_type BYTE_IVEC3 = {GL_FALSE, GL_BYTE, 3, 3, true};
        constexpr vertex_element_type BYTE_IVEC4 = {GL_FALSE, GL_BYTE, 4, 4, true};

        constexpr vertex_element_type BYTE_FLOAT = {GL_FALSE, GL_BYTE, 1, 1, false};
        constexpr vertex_element_type BYTE_VEC2 = {GL_FALSE, GL_BYTE, 2, 2, false};
        constexpr vertex_element_type BYTE_VEC3 = {GL_FALSE, GL_BYTE, 3, 3, false};
        constexpr vertex_element_type BYTE_VEC4 = {GL_FALSE, GL_BYTE, 4, 4, false};

        constexpr vertex_element_type BYTE_NFLOAT = {GL_TRUE, GL_BYTE, 1, 1, false};
        constexpr vertex_element_type BYTE_NVEC2 = {GL_TRUE, GL_BYTE, 2, 2, false};
        constexpr vertex_element_type BYTE_NVEC3 = {GL_TRUE, GL_BYTE, 3, 3, false};
        constexpr vertex_element_type BYTE_NVEC4 = {GL_TRUE, GL_BYTE, 4, 4, false};


        constexpr vertex_element_type SHORT = {GL_FALSE, GL_SHORT, 1, 2, true};
        constexpr vertex_element_type SHORT_IVEC2 = {GL_FALSE, GL_SHORT, 2, 4, true};
        constexpr vertex_element_type SHORT_IVEC3 = {GL_FALSE, GL_SHORT, 3, 6, true};
        constexpr vertex_element_type SHORT_IVEC4 = {GL_FALSE, GL_SHORT, 4, 8, true};

        constexpr vertex_element_type SHORT_FLOAT = {GL_FALSE, GL_SHORT, 1, 2, false};
        constexpr vertex_element_type SHORT_VEC2 = {GL_FALSE, GL_SHORT, 2, 4, false};
        constexpr vertex_element_type SHORT_VEC3 = {GL_FALSE, GL_SHORT, 3, 6, false};
        constexpr vertex_element_type SHORT_VEC4 = {GL_FALSE, GL_SHORT, 4, 8, false};

        constexpr vertex_element_type SHORT_NFLOAT = {GL_TRUE, GL_SHORT, 1, 2, false};
        constexpr vertex_element_type SHORT_NVEC2 = {GL_TRUE, GL_SHORT, 2, 4, false};
        constexpr vertex_element_type SHORT_NVEC3 = {GL_TRUE, GL_SHORT, 3, 6, false};
        constexpr vertex_element_type SHORT_NVEC4 = {GL_TRUE, GL_SHORT, 4, 8, false};


        constexpr vertex_element_type INT = {GL_FALSE, GL_INT, 1, 4, true};
        constexpr vertex_element_type IVEC2 = {GL_FALSE, GL_INT, 2, 8, true};
        constexpr vertex_element_type IVEC3 = {GL_FALSE, GL_INT, 3, 12, true};
        constexpr vertex_element_type IVEC4 = {GL_FALSE, GL_INT, 4, 16, true};

        constexpr vertex_element_type INT_FLOAT = {GL_FALSE, GL_INT, 1, 4, false};
        constexpr vertex_element_type INT_VEC2 = {GL_FALSE, GL_INT, 2, 8, false};
        constexpr vertex_element_type INT_VEC3 = {GL_FALSE, GL_INT, 3, 12, false};
        constexpr vertex_element_type INT_VEC4 = {GL_FALSE, GL_INT, 4, 16, false};

        constexpr vertex_element_type INT_NFLOAT = {GL_TRUE, GL_INT, 1, 4, false};
        constexpr vertex_element_type INT_NVEC2 = {GL_TRUE, GL_INT, 2, 8, false};
        constexpr vertex_element_type INT_NVEC3 = {GL_TRUE, GL_INT, 3, 12, false};
        constexpr vertex_element_type INT_NVEC4 = {GL_TRUE, GL_INT, 4, 16, false};


        constexpr vertex_element_type HALF = {GL_FALSE, GL_HALF_FLOAT, 1, 2, false};
        constexpr vertex_element_type HALF_VEC2 = {GL_FALSE, GL_HALF_FLOAT, 2, 4, false};
        constexpr vertex_element_type HALF_VEC3 = {GL_FALSE, GL_HALF_FLOAT, 3, 6, false};
        constexpr vertex_element_type HALF_VEC4 = {GL_FALSE, GL_HALF_FLOAT, 4, 8, false};

        constexpr vertex_element_type FLOAT = {GL_FALSE, GL_FLOAT, 1, 4, false};
        constexpr vertex_element_type VEC2 = {GL_FALSE, GL_FLOAT, 2, 8, false};
        constexpr vertex_element_type VEC3 = {GL_FALSE, GL_FLOAT, 3, 12, false};
        constexpr vertex_element_type VEC4 = {GL_FALSE, GL_FLOAT, 4, 16, false};

        constexpr vertex_element_type DOUBLE = {GL_FALSE, GL_DOUBLE, 1, 8, false};
        constexpr vertex_element_type DVEC2 = {GL_FALSE, GL_DOUBLE, 2, 16, false};
        constexpr vertex_element_type DVEC3 = {GL_FALSE, GL_DOUBLE, 3, 24, false};
        constexpr vertex_element_type DVEC4 = {GL_FALSE, GL_DOUBLE, 4, 32, false};
    }
}
