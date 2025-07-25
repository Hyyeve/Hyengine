#pragma once
#include "../library/gl.hpp"
#include <array>

namespace hyengine
{
    struct vertex_element
    {
        GLboolean normalized;
        GLenum gl_type;
        int count;
        int bytes;
        bool asInt;
        int instance_divisor;
    };

    template <size_t size>
    class vertex_format
    {
    public:
        constexpr explicit vertex_format(const std::array<vertex_element, size> elements) noexcept : elements(elements)
        {
            for (const vertex_element& element : elements) total_bytes += element.bytes;
        }

        void apply(const GLuint vertex_format_buffer, const GLuint vertex_buffer_slot) const
        {
            size_t offset = 0;
            for (int i = 0; i < elements.size(); i++)
            {
                const vertex_element& element = elements[i];
                glEnableVertexArrayAttrib(vertex_format_buffer, i);

                if (element.asInt)
                {
                    glVertexArrayAttribIFormat(vertex_format_buffer, i, element.count, element.gl_type, offset);
                }
                else if (element.gl_type == GL_DOUBLE)
                {
                    glVertexArrayAttribLFormat(vertex_format_buffer, i, element.count, element.gl_type, offset);
                }
                else
                {
                    glVertexArrayAttribFormat(vertex_format_buffer, i, element.count, element.gl_type, element.normalized, offset);
                }

                glVertexArrayAttribBinding(vertex_format_buffer, i, vertex_buffer_slot);

                if (element.instance_divisor > 0)
                {
                    glVertexArrayBindingDivisor(vertex_format_buffer, i, element.instance_divisor);
                }

                offset += element.bytes;
            }
        }

    private:
        std::array<vertex_element, size> elements;
        int total_bytes = 0;
    };

    namespace vertex_element_types
    {
        constexpr vertex_element make_instanced(const vertex_element& element, const int instance_divisor)
        {
            return vertex_element{element.normalized, element.gl_type, element.count, element.bytes, element.asInt, instance_divisor};
        }

        constexpr vertex_element UBYTE = {GL_FALSE, GL_UNSIGNED_BYTE, 1, 1, true, 0};
        constexpr vertex_element UBYTE_UVEC2 = {GL_FALSE, GL_UNSIGNED_BYTE, 2, 2, true, 0};
        constexpr vertex_element UBYTE_UVEC3 = {GL_FALSE, GL_UNSIGNED_BYTE, 3, 3, true, 0};
        constexpr vertex_element UBYTE_UVEC4 = {GL_FALSE, GL_UNSIGNED_BYTE, 4, 4, true, 0};

        constexpr vertex_element UBYTE_FLOAT = {GL_FALSE, GL_UNSIGNED_BYTE, 1, 1, false, 0};
        constexpr vertex_element UBYTE_VEC2 = {GL_FALSE, GL_UNSIGNED_BYTE, 2, 2, false, 0};
        constexpr vertex_element UBYTE_VEC3 = {GL_FALSE, GL_UNSIGNED_BYTE, 3, 3, false, 0};
        constexpr vertex_element UBYTE_VEC4 = {GL_FALSE, GL_UNSIGNED_BYTE, 4, 4, false, 0};

        constexpr vertex_element UBYTE_NFLOAT = {GL_TRUE, GL_UNSIGNED_BYTE, 1, 1, false, 0};
        constexpr vertex_element UBYTE_NVEC2 = {GL_TRUE, GL_UNSIGNED_BYTE, 2, 2, false, 0};
        constexpr vertex_element UBYTE_NVEC3 = {GL_TRUE, GL_UNSIGNED_BYTE, 3, 3, false, 0};
        constexpr vertex_element UBYTE_NVEC4 = {GL_TRUE, GL_UNSIGNED_BYTE, 4, 4, false, 0};


        constexpr vertex_element USHORT = {GL_FALSE, GL_UNSIGNED_SHORT, 1, 2, true, 0};
        constexpr vertex_element USHORT_UVEC2 = {GL_FALSE, GL_UNSIGNED_SHORT, 2, 4, true, 0};
        constexpr vertex_element USHORT_UVEC3 = {GL_FALSE, GL_UNSIGNED_SHORT, 3, 6, true, 0};
        constexpr vertex_element USHORT_UVEC4 = {GL_FALSE, GL_UNSIGNED_SHORT, 4, 8, true, 0};

        constexpr vertex_element USHORT_FLOAT = {GL_FALSE, GL_UNSIGNED_SHORT, 1, 2, false, 0};
        constexpr vertex_element USHORT_VEC2 = {GL_FALSE, GL_UNSIGNED_SHORT, 2, 4, false, 0};
        constexpr vertex_element USHORT_VEC3 = {GL_FALSE, GL_UNSIGNED_SHORT, 3, 6, false, 0};
        constexpr vertex_element USHORT_VEC4 = {GL_FALSE, GL_UNSIGNED_SHORT, 4, 8, false, 0};

        constexpr vertex_element USHORT_NFLOAT = {GL_TRUE, GL_UNSIGNED_SHORT, 1, 2, false, 0};
        constexpr vertex_element USHORT_NVEC2 = {GL_TRUE, GL_UNSIGNED_SHORT, 2, 4, false, 0};
        constexpr vertex_element USHORT_NVEC3 = {GL_TRUE, GL_UNSIGNED_SHORT, 3, 6, false, 0};
        constexpr vertex_element USHORT_NVEC4 = {GL_TRUE, GL_UNSIGNED_SHORT, 4, 8, false, 0};


        constexpr vertex_element UINT = {GL_FALSE, GL_UNSIGNED_INT, 1, 4, true, 0};
        constexpr vertex_element UVEC2 = {GL_FALSE, GL_UNSIGNED_INT, 2, 8, true, 0};
        constexpr vertex_element UVEC3 = {GL_FALSE, GL_UNSIGNED_INT, 3, 12, true, 0};
        constexpr vertex_element UVEC4 = {GL_FALSE, GL_UNSIGNED_INT, 4, 16, true, 0};

        constexpr vertex_element UINT_FLOAT = {GL_FALSE, GL_UNSIGNED_INT, 1, 4, false, 0};
        constexpr vertex_element UINT_VEC2 = {GL_FALSE, GL_UNSIGNED_INT, 2, 8, false, 0};
        constexpr vertex_element UINT_VEC3 = {GL_FALSE, GL_UNSIGNED_INT, 3, 12, false, 0};
        constexpr vertex_element UINT_VEC4 = {GL_FALSE, GL_UNSIGNED_INT, 4, 16, false, 0};

        constexpr vertex_element UINT_NFLOAT = {GL_TRUE, GL_UNSIGNED_INT, 1, 4, false, 0};
        constexpr vertex_element UINT_NVEC2 = {GL_TRUE, GL_UNSIGNED_INT, 2, 8, false, 0};
        constexpr vertex_element UINT_NVEC3 = {GL_TRUE, GL_UNSIGNED_INT, 3, 12, false, 0};
        constexpr vertex_element UINT_NVEC4 = {GL_TRUE, GL_UNSIGNED_INT, 4, 16, false, 0};


        constexpr vertex_element BYTE = {GL_FALSE, GL_BYTE, 1, 1, true, 0};
        constexpr vertex_element BYTE_IVEC2 = {GL_FALSE, GL_BYTE, 2, 2, true, 0};
        constexpr vertex_element BYTE_IVEC3 = {GL_FALSE, GL_BYTE, 3, 3, true, 0};
        constexpr vertex_element BYTE_IVEC4 = {GL_FALSE, GL_BYTE, 4, 4, true, 0};

        constexpr vertex_element BYTE_FLOAT = {GL_FALSE, GL_BYTE, 1, 1, false, 0};
        constexpr vertex_element BYTE_VEC2 = {GL_FALSE, GL_BYTE, 2, 2, false, 0};
        constexpr vertex_element BYTE_VEC3 = {GL_FALSE, GL_BYTE, 3, 3, false, 0};
        constexpr vertex_element BYTE_VEC4 = {GL_FALSE, GL_BYTE, 4, 4, false, 0};

        constexpr vertex_element BYTE_NFLOAT = {GL_TRUE, GL_BYTE, 1, 1, false, 0};
        constexpr vertex_element BYTE_NVEC2 = {GL_TRUE, GL_BYTE, 2, 2, false, 0};
        constexpr vertex_element BYTE_NVEC3 = {GL_TRUE, GL_BYTE, 3, 3, false, 0};
        constexpr vertex_element BYTE_NVEC4 = {GL_TRUE, GL_BYTE, 4, 4, false, 0};


        constexpr vertex_element SHORT = {GL_FALSE, GL_SHORT, 1, 2, true, 0};
        constexpr vertex_element SHORT_IVEC2 = {GL_FALSE, GL_SHORT, 2, 4, true, 0};
        constexpr vertex_element SHORT_IVEC3 = {GL_FALSE, GL_SHORT, 3, 6, true, 0};
        constexpr vertex_element SHORT_IVEC4 = {GL_FALSE, GL_SHORT, 4, 8, true, 0};

        constexpr vertex_element SHORT_FLOAT = {GL_FALSE, GL_SHORT, 1, 2, false, 0};
        constexpr vertex_element SHORT_VEC2 = {GL_FALSE, GL_SHORT, 2, 4, false, 0};
        constexpr vertex_element SHORT_VEC3 = {GL_FALSE, GL_SHORT, 3, 6, false, 0};
        constexpr vertex_element SHORT_VEC4 = {GL_FALSE, GL_SHORT, 4, 8, false, 0};

        constexpr vertex_element SHORT_NFLOAT = {GL_TRUE, GL_SHORT, 1, 2, false, 0};
        constexpr vertex_element SHORT_NVEC2 = {GL_TRUE, GL_SHORT, 2, 4, false, 0};
        constexpr vertex_element SHORT_NVEC3 = {GL_TRUE, GL_SHORT, 3, 6, false, 0};
        constexpr vertex_element SHORT_NVEC4 = {GL_TRUE, GL_SHORT, 4, 8, false, 0};


        constexpr vertex_element INT = {GL_FALSE, GL_INT, 1, 4, true, 0};
        constexpr vertex_element IVEC2 = {GL_FALSE, GL_INT, 2, 8, true, 0};
        constexpr vertex_element IVEC3 = {GL_FALSE, GL_INT, 3, 12, true, 0};
        constexpr vertex_element IVEC4 = {GL_FALSE, GL_INT, 4, 16, true, 0};

        constexpr vertex_element INT_FLOAT = {GL_FALSE, GL_INT, 1, 4, false, 0};
        constexpr vertex_element INT_VEC2 = {GL_FALSE, GL_INT, 2, 8, false, 0};
        constexpr vertex_element INT_VEC3 = {GL_FALSE, GL_INT, 3, 12, false, 0};
        constexpr vertex_element INT_VEC4 = {GL_FALSE, GL_INT, 4, 16, false, 0};

        constexpr vertex_element INT_NFLOAT = {GL_TRUE, GL_INT, 1, 4, false, 0};
        constexpr vertex_element INT_NVEC2 = {GL_TRUE, GL_INT, 2, 8, false, 0};
        constexpr vertex_element INT_NVEC3 = {GL_TRUE, GL_INT, 3, 12, false, 0};
        constexpr vertex_element INT_NVEC4 = {GL_TRUE, GL_INT, 4, 16, false, 0};


        constexpr vertex_element HALF = {GL_FALSE, GL_HALF_FLOAT, 1, 2, false, 0};
        constexpr vertex_element HALF_VEC2 = {GL_FALSE, GL_HALF_FLOAT, 2, 4, false, 0};
        constexpr vertex_element HALF_VEC3 = {GL_FALSE, GL_HALF_FLOAT, 3, 6, false, 0};
        constexpr vertex_element HALF_VEC4 = {GL_FALSE, GL_HALF_FLOAT, 4, 8, false, 0};

        constexpr vertex_element FLOAT = {GL_FALSE, GL_FLOAT, 1, 4, false, 0};
        constexpr vertex_element VEC2 = {GL_FALSE, GL_FLOAT, 2, 8, false, 0};
        constexpr vertex_element VEC3 = {GL_FALSE, GL_FLOAT, 3, 12, false, 0};
        constexpr vertex_element VEC4 = {GL_FALSE, GL_FLOAT, 4, 16, false, 0};

        constexpr vertex_element DOUBLE = {GL_FALSE, GL_DOUBLE, 1, 8, false, 0};
        constexpr vertex_element DVEC2 = {GL_FALSE, GL_DOUBLE, 2, 16, false, 0};
        constexpr vertex_element DVEC3 = {GL_FALSE, GL_DOUBLE, 3, 24, false, 0};
        constexpr vertex_element DVEC4 = {GL_FALSE, GL_DOUBLE, 4, 32, false, 0};
    }
}
