#pragma once
#include <string>

#include "types.hpp"

namespace stardraw
{
    ///NOTE: Buffer memory storage cannot be guarenteed on OpenGL, but SYSRAM guarentees it will be possible to write into the buffer directly.
    enum class buffer_memory_storage : uint8_t
    {
        SYSRAM, VRAM,
    };

    struct buffer_descriptor final : descriptor
    {
        explicit buffer_descriptor(const std::string_view& name, const uint64_t size, const buffer_memory_storage hint = buffer_memory_storage::VRAM) : descriptor(name), size(size), hint(hint) {}

        [[nodiscard]] descriptor_type type() const override
        {
            return descriptor_type::BUFFER;
        }

        uint64_t size;
        buffer_memory_storage hint;
    };

    enum vertex_element_type : uint8_t
    {
        UINT_U8, UNT2_U8, UINT3_U8, UINT4_U8,
        UINT_U16, UINT2_U16, UINT3_U16, UINT4_U16,
        UINT_U32, UINT2_U32, UINT3_U32, UINT4_U32,

        INT_I8, INT2_I8, INT3_I8, INT4_I8,
        INT_I16, INT2_I16, INT3_I16, INT4_I16,
        INT_I32, INT2_I32, INT3_I32, INT4_I32,

        FLOAT_U8_NORM, FLOAT2_U8_NORM, FLOAT3_U8_NORM, FLOAT4_U8_NORM,
        FLOAT_I8_NORM, FLOAT2_I8_NORM, FLOAT3_I8_NORM, FLOAT4_I8_NORM,

        FLOAT_U16_NORM, FLOAT2_U16_NORM, FLOAT3_U16_NORM, FLOAT4_U16_NORM,
        FLOAT_I16_NORM, FLOAT2_I16_NORM, FLOAT3_I16_NORM, FLOAT4_I16_NORM,
        FLOAT_F16, FLOAT2_F16, FLOAT3_F16, FLOAT4_F16,

        FLOAT_F32, FLOAT2_F32, FLOAT3_F32, FLOAT4_F32
    };

    struct vertex_element
    {
        constexpr vertex_element(const std::string_view& buffer_source, const vertex_element_type& type, const uint32_t instance_divisor = 0) : type(type), instance_divisor(instance_divisor), buffer_source(buffer_source) {}

        vertex_element_type type;
        uint32_t instance_divisor;
        std::string buffer_source;
    };

    struct vertex_format_specification
    {
        constexpr vertex_format_specification(const std::initializer_list<vertex_element> elements) : elements(elements) {}
        explicit constexpr vertex_format_specification(const std::vector<vertex_element>& elements) : elements(elements) {}

        std::vector<vertex_element> elements;
    };

    struct vertex_buffers_specification
    {
        constexpr vertex_buffers_specification(const std::initializer_list<std::string> vertex_sources, const std::string_view& index_source) : index_source(index_source), vertex_sources(vertex_sources) {}
        constexpr vertex_buffers_specification(const std::initializer_list<std::string> vertex_sources) : index_source(""), vertex_sources(vertex_sources) {}
        constexpr vertex_buffers_specification(const std::vector<std::string>& vertex_sources, const std::string_view& index_source) : index_source(index_source), vertex_sources(vertex_sources) {}

        std::string index_source;
        std::vector<std::string> vertex_sources;
    };

    struct vertex_specification_descriptor final : descriptor
    {
        constexpr vertex_specification_descriptor(const std::string_view& name, const vertex_buffers_specification& buffers, const vertex_format_specification& elements) : descriptor(name), buffers(buffers), format(elements) {}

        [[nodiscard]] descriptor_type type() const override
        {
            return descriptor_type::VERTEX_SPECIFICATION;
        }

        vertex_buffers_specification buffers;
        vertex_format_specification format;
    };
}
