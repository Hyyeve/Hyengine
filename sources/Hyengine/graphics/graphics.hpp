#pragma once

#include "../library/glm.hpp"
#include "../library/gl.hpp"

#include "../common/math/rectangle.hpp"

namespace hyengine
{
    struct gl_draw_indirect_cmd
    {
        unsigned int vertex_count;
        unsigned int instance_count;
        unsigned int vertex_begin;
        unsigned int instance_begin;
    };

    struct viewport
    {
        int x_offset, y_offset;
        unsigned int width, height;
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
        constexpr blending_config alpha_blend = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
        constexpr blending_config additive_blend = {GL_ONE, GL_ONE, GL_ONE, GL_ONE};
        constexpr blending_config subtractive_blend = {GL_ONE, GL_ONE, GL_ONE, GL_ONE, GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT};
        constexpr blending_config overwrite_blend = {GL_ONE, GL_ZERO, GL_ONE, GL_ZERO};
    }

    [[nodiscard]] bool try_init_glfw();
    [[nodiscard]] bool try_load_glad();

    void enable_gl_debug();

    void enable_scissor_test();
    void disable_scissor_test();

    void set_scissor(const int x, const int y, const int width, const int height);
    void set_scissor(const glm::ivec2 position, const glm::ivec2 size);
    void set_scissor(const rectangle& rect);

    void set_viewport(const viewport& viewport);
    [[nodiscard]] const viewport& get_viewport();
    [[nodiscard]] viewport create_letterbox_viewport(const viewport& to_fit, const int target_width, const int target_height, const bool fill);

    void set_gl_flag_enabled(const GLenum setting, const bool enable);

    void set_blending_config(const blending_config& config, const unsigned int buffer_slot = 0);
    void set_blending_enabled(const bool enable);

    void set_stencil_config(const stencil_config& config, const GLenum facing);
    void set_stencil_enabled(const bool enable);

    void set_cull_face_enabled(const bool enable);
    void set_cull_face_mode(const GLenum mode);

    void set_depth_test_enabled(const bool enable);

    void set_cubemap_seamless_sampling(const bool enable);

    void set_clear_color(const glm::vec4 color);
    void set_clear_depth(const float depth);
    void set_clear_stencil(const int stencil);
    void clear_buffers(const GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    [[nodiscard]] unsigned int get_max_texture_units();
}
