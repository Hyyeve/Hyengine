#pragma once
#include <string>
#include <string_view>

#include "stardraw/api/types.hpp"

namespace stardraw
{
    enum class draw_mode : uint8_t
    {
        TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN,
    };

    enum class draw_indexed_index_type : uint8_t
    {
        UINT_32, UINT_16, UINT_8
    };

    struct draw_command final : command
    {
        draw_command(const std::string_view& vertex_source, const draw_mode mode, const uint32_t count, const uint32_t offset = 0, const uint32_t instances = 1, const uint32_t instance_offset = 0) : vertex_source(vertex_source), mode(mode), count(count), vertex_source_offset(offset), instances(instances), instance_offset(instance_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW;
        }

        object_identifier vertex_source;
        draw_mode mode;
        uint32_t count;
        ///Starting index for vertices
        uint32_t vertex_source_offset;

        uint32_t instances;
        ///Starting index for instanced attributes
        uint32_t instance_offset = 0;
    };

    struct draw_indexed_command final : command
    {
        draw_indexed_command(const std::string_view& vertex_source, const std::string_view& index_source, const draw_mode mode, const uint32_t count, const int32_t offset = 0, const uint32_t index_offset = 0, const uint32_t instances = 1, const uint32_t instance_offset = 0, const draw_indexed_index_type index_type = draw_indexed_index_type::UINT_32) : vertex_source(vertex_source), index_source(index_source), mode(mode), index_type(index_type), count(count), vertex_source_offset(offset), index_source_offset(index_offset), instances(instances), instance_offset(instance_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDEXED;
        }

        object_identifier vertex_source;
        object_identifier index_source;

        draw_mode mode;
        draw_indexed_index_type index_type;
        uint32_t count;

        ///Offset applied to all vertex indices
        int32_t vertex_source_offset;

        ///Starting index for indices
        uint32_t index_source_offset;

        uint32_t instances;
        ///Starting index for instanced attributes
        uint32_t instance_offset;
    };

    struct draw_indirect_command final : command
    {
        draw_indirect_command(const std::string_view& vertex_source, const std::string_view& indirect_source, const draw_mode mode, const uint32_t draw_count, const uint32_t indirect_source_offset = 0) : vertex_source(vertex_source), indirect_source(indirect_source), mode(mode), draw_count(draw_count), indirect_source_offset(indirect_source_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDIRECT;
        }

        object_identifier vertex_source;
        object_identifier indirect_source;

        draw_mode mode;
        uint32_t draw_count;
        uint32_t indirect_source_offset;
    };

    struct draw_indexed_indirect_command final : command
    {
        draw_indexed_indirect_command(const std::string_view& vertex_source, const std::string_view& index_source, const std::string_view& indirect_source, const draw_mode mode, const uint32_t draw_count, const uint32_t indirect_source_offset = 0, const draw_indexed_index_type index_type = draw_indexed_index_type::UINT_32) : vertex_source(vertex_source), index_source(index_source), indirect_source(indirect_source), mode(mode), index_type(index_type), draw_count(draw_count), indirect_source_offset(indirect_source_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDIRECT;
        }

        object_identifier vertex_source;
        object_identifier index_source;
        object_identifier indirect_source;

        draw_mode mode;
        draw_indexed_index_type index_type;
        uint32_t draw_count;
        uint32_t indirect_source_offset;
    };

    enum class stencil_test_func : uint8_t
    {
        ALWAYS, NEVER, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL
    };

    enum class stencil_result_op : uint8_t
    {
        KEEP, ZERO, REPLACE, INCREMENT, INCREMENT_WRAP, DECREMENT, DECREMENT_WRAP, INVERT
    };

    enum class triangle_facing : uint8_t
    {
        FRONT, BACK, BOTH
    };

    struct stencil_config
    {
        stencil_test_func test_func = stencil_test_func::ALWAYS;

        stencil_result_op stencil_fail_op = stencil_result_op::KEEP;
        stencil_result_op depth_fail_op = stencil_result_op::KEEP;
        stencil_result_op pixel_pass_op = stencil_result_op::KEEP;

        int reference = 0;
        int test_mask = std::numeric_limits<int>::max();
        int write_mask = std::numeric_limits<int>::max();
    };

    struct config_stencil_command final : command
    {
        explicit config_stencil_command(const stencil_config& config, const triangle_facing for_facing) : config(config), for_facing(for_facing) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::CONFIG_STENCIL;
        }

        stencil_config config;
        triangle_facing for_facing;
    };

    enum class blending_func : uint8_t
    {
        ADD, SUBTRACT, REVERSE_SUBTRACT, MIN, MAX
    };

    enum class blending_factor : uint8_t
    {
        ZERO,
        ONE,

        CONSTANT_COLOR,
        CONSTANT_ALPHA,

        ONE_MINUS_CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_ALPHA,

        SOURCE_COLOR,
        DEST_COLOR,

        ONE_MINUS_SOURCE_COLOR,
        ONE_MINUS_DEST_COLOR,
        SOURCE_ALPHA,
        DEST_ALPHA,

        ONE_MINUS_SOURCE_ALPHA,
        ONE_MINUS_DEST_ALPHA,

        SOURCE_ALPHA_SATURATE,

        SECONDARY_SOURCE_COLOR,
        SECONDARY_SOURCE_ALPHA,
    };

    struct blending_config
    {
        blending_factor source_blend_rgb = blending_factor::SOURCE_ALPHA;
        blending_factor dest_blend_rgb = blending_factor::ONE_MINUS_SOURCE_ALPHA;
        blending_func rgb_equation = blending_func::ADD;

        blending_factor source_blend_alpha = blending_factor::SOURCE_ALPHA;
        blending_factor dest_blend_alpha = blending_factor::ONE_MINUS_SOURCE_ALPHA;
        blending_func alpha_equation = blending_func::ADD;

        float constant_blend_r = 1.0f;
        float constant_blend_g = 1.0f;
        float constant_blend_b = 1.0f;
        float constant_blend_a = 1.0f;
    };

    namespace standard_blending_configs
    {
        constexpr blending_config ALPHA = {};
        constexpr blending_config OVERWRITE = {blending_factor::ONE, blending_factor::ZERO, blending_func::ADD, blending_factor::ONE, blending_factor::ZERO};
        constexpr blending_config ADDITIVE = {blending_factor::ONE, blending_factor::ONE};
        constexpr blending_config SUBTRACTIVE = {blending_factor::ONE, blending_factor::ONE, blending_func::REVERSE_SUBTRACT};
        constexpr blending_config MULTIPLY = {blending_factor::DEST_COLOR, blending_factor::ZERO};
        constexpr blending_config DARKEN = {blending_factor::ONE, blending_factor::ONE, blending_func::MIN};
        constexpr blending_config LIGHTEN = {blending_factor::ONE, blending_factor::ONE, blending_func::MAX};
    }

    struct config_blending_command final : command
    {
        explicit config_blending_command(const blending_config& config, const uint32_t draw_buffer_index = 0) : config(config), draw_buffer_index(draw_buffer_index) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::CONFIG_BLENDING;
        }

        blending_config config;
        uint32_t draw_buffer_index;
    };

    struct buffer_sync_command final : command
    {
        explicit buffer_sync_command(const std::string_view buffer_source) : buffer_identifier(buffer_source) {}
        [[nodiscard]] command_type type() const override
        {
            return command_type::BUFFER_SYNC;
        }

        object_identifier buffer_identifier;
    };

    struct buffer_upload_command final : command
    {
        explicit buffer_upload_command(const std::string_view& buffer_source, const uint64_t bytes, const uint64_t upload_bytes, const void* const data) : buffer_identifier(buffer_source), upload_address(bytes), upload_bytes(upload_bytes), upload_data(data) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::BUFFER_UPLOAD;
        }

        object_identifier buffer_identifier;
        uint64_t upload_address;
        uint64_t upload_bytes;
        const void* const upload_data;
    };

    struct buffer_copy_command final : command
    {
        explicit buffer_copy_command(const std::string_view& from_source, const std::string_view& to_source, const uint64_t from_address, const uint64_t to_address, const uint64_t bytes) : source_identifier(from_source), dest_identifier(to_source), source_address(from_address), dest_address(to_address), bytes(bytes) {}
        [[nodiscard]] command_type type() const override
        {
            return command_type::BUFFER_COPY;
        }

        object_identifier source_identifier;
        object_identifier dest_identifier;
        uint64_t source_address;
        uint64_t dest_address;
        uint64_t bytes;
    };

    enum class buffer_attachment_type : uint8_t
    {
        SHADER_STORAGE_BLOCK, SHADER_UNIFORM_BLOCK, SHADER_ATOMIC_COUNTER_BLOCK,
    };

    struct buffer_attach_command final : command
    {
        explicit buffer_attach_command(const std::string_view& buffer_name, const buffer_attachment_type attachment_type, const uint32_t attachment_index) : buffer_identifier(buffer_name), attachment_type(attachment_type), attachment_index(attachment_index) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::BUFFER_ATTACH;
        }

        object_identifier buffer_identifier;
        buffer_attachment_type attachment_type;
        uint32_t attachment_index;
    };
}
