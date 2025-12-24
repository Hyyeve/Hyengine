#pragma once

#include "../library/gl.hpp"
#include "../library/glm.hpp"

#include "../common/sized_numerics.hpp"
#include "../common/math/aa_rectangle.hpp"

namespace hyengine
{
    #pragma pack(push, 1)
    struct gl_draw_arrays_indirect_cmd
    {
        u32 vertex_count;
        u32 instance_count;
        u32 vertex_begin;
        u32 instance_begin;
    };

    struct gl_draw_elements_indirect_cmd
    {
        u32 vertex_count;
        u32 instance_count;
        u32 index_begin;
        i32 vertex_begin;
        u32 instance_begin;
    };
    #pragma pack(pop)

    struct render_viewport
    {
        i32 x_offset, y_offset;
        u32 width, height;
    };

    struct blending_config
    {
        GLenum source_blend_rgb;
        GLenum dest_blend_rgb;

        GLenum source_blend_alpha;
        GLenum dest_blend_alpha;

        GLenum rgb_equation = GL_FUNC_ADD;
        GLenum alpha_equation = GL_FUNC_ADD;

        glm::vec4 constant_blend_color = glm::vec4(1.0);
    };

    struct stencil_config
    {
        GLenum test_func = GL_ALWAYS;

        GLenum stencil_fail_op = GL_KEEP;
        GLenum depth_fail_op = GL_KEEP;
        GLenum pixel_pass_op = GL_REPLACE;

        GLint reference = 0;
        GLint test_mask = std::numeric_limits<GLint>::max();
        GLint write_mask = std::numeric_limits<GLint>::max();
    };

    namespace blending_configs
    {
        constexpr blending_config ALPHA_BLEND = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
        constexpr blending_config ADDITIVE_BLEND = {GL_ONE, GL_ONE, GL_ONE, GL_ONE};
        constexpr blending_config SUBTRACTIVE_BLEND = {GL_ONE, GL_ONE, GL_ONE, GL_ONE, GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT};
        constexpr blending_config OVERWRITE_BLEND = {GL_ONE, GL_ZERO, GL_ONE, GL_ZERO};
    }

    [[nodiscard]] bool try_init_glfw();
    [[nodiscard]] bool try_load_glad();

    void enable_gl_debug();

    void enable_scissor_test();
    void disable_scissor_test();

    void set_scissor(const i32 x, const i32 y, const i32 width, const i32 height);
    void set_scissor(const glm::ivec2 position, const glm::ivec2 size);
    void set_scissor(const aa_rectangle& rect);

    void set_viewport(const render_viewport& viewport);
    [[nodiscard]] const render_viewport& get_viewport();
    [[nodiscard]] render_viewport create_fitted_viewport(const render_viewport& to_fit, const i32 target_width, const i32 target_height, const bool fill);

    void set_gl_flag_enabled(const GLenum setting, const bool enable);
    [[nodiscard]] bool get_gl_flag_enabled(const GLenum setting);
    [[nodiscard]] i32 get_gl_const_i32(const GLenum constant);

    void set_blending_config(const blending_config& config, const u32 buffer_slot = 0);
    void set_blending_enabled(const bool enable);

    void set_stencil_config(const stencil_config& config, const GLenum facing);
    void set_stencil_enabled(const bool enable);

    void set_cull_face_enabled(const bool enable);
    void set_cull_face_mode(const GLenum mode);

    void set_depth_test_enabled(const bool enable);

    void set_cubemap_seamless_sampling(const bool enable);

    void set_clear_color(const glm::vec4 color);
    void set_clear_depth(const f32 depth);
    void set_clear_stencil(const i32 stencil);
    void clear_buffers(const GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
