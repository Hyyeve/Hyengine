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

    struct draw_command final : command
    {
        draw_command(const std::string_view vertex_source, const draw_mode mode, const uint32_t count, const uint32_t offset = 0, const uint32_t instances = 1, const uint32_t instance_offset = 0) : vertex_source(vertex_source), mode(mode), count(count), vertex_source_offset(offset), instances(instances), instance_offset(instance_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW;
        }

        std::string vertex_source;
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
        draw_indexed_command(const std::string_view vertex_source, const std::string_view index_source, const draw_mode mode, const uint32_t count, const int32_t offset = 0, const uint32_t index_offset = 0, const uint32_t instances = 1, const uint32_t instance_offset = 0) : vertex_source(vertex_source), index_source(index_source), mode(mode), count(count), vertex_source_offset(offset), index_source_offset(index_offset), instances(instances), instance_offset(instance_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDEXED;
        }

        std::string vertex_source;
        std::string index_source;

        draw_mode mode;
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
        draw_indirect_command(const std::string_view vertex_source, const std::string_view indirect_source, const draw_mode mode, const uint32_t draw_count, const uint32_t indirect_source_offset = 0) : vertex_source(vertex_source), indirect_source(indirect_source), mode(mode), draw_count(draw_count), indirect_source_offset(indirect_source_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDIRECT;
        }

        std::string vertex_source;
        std::string indirect_source;

        draw_mode mode;
        uint32_t draw_count;
        uint32_t indirect_source_offset;
    };

    struct draw_indexed_indirect_command final : command
    {
        draw_indexed_indirect_command(const std::string_view vertex_source, const std::string_view index_source, const std::string_view indirect_source, const draw_mode mode, const uint32_t draw_count, const uint32_t indirect_source_offset = 0) : vertex_source(vertex_source), index_source(index_source), indirect_source(indirect_source), mode(mode), draw_count(draw_count), indirect_source_offset(indirect_source_offset) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::DRAW_INDIRECT;
        }

        std::string vertex_source;
        std::string index_source;
        std::string indirect_source;

        draw_mode mode;
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
        blending_factor source_blend_rgb;
        blending_factor dest_blend_rgb;

        blending_factor source_blend_alpha;
        blending_factor dest_blend_alpha;

        blending_func rgb_equation = blending_func::ADD;
        blending_func alpha_equation = blending_func::ADD;

        float constant_blend_r = 1.0f;
        float constant_blend_g = 1.0f;
        float constant_blend_b = 1.0f;
        float constant_blend_a = 1.0f;
    };

    struct config_blending_command final : command
    {
        explicit config_blending_command(const blending_config& config, const uint32_t draw_buffer_index) : config(config), draw_buffer_index(draw_buffer_index) {}

        [[nodiscard]] command_type type() const override
        {
            return command_type::CONFIG_BLENDING;
        }

        blending_config config;
        uint32_t draw_buffer_index;
    };
}
