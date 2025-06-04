#pragma once

#include "../library/glm.hpp"
#include "../library/gl.hpp"

#include "../common/math/rectangle.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    enum class ScissorMode : std::uint8_t {
        OVERWRITE, UNION, INTERSECTION
    };

    struct gl_draw_indirect_cmd {
        unsigned int vertex_count;
        unsigned int instance_count;
        unsigned int vertex_begin;
        unsigned int instance_begin;
    };

    struct viewport {
        int x_offset, y_offset;
        unsigned int width, height;
    };

    struct blend_mode
    {
        GLenum rgb_equation;
        GLenum alpha_equation;

    };

    [[nodiscard]] bool try_init_glfw();
    [[nodiscard]] bool try_load_glad();

    void enable_gl_debug();

    void enable_scissor_test();
    void disable_scissor_test();

    void set_scissor(const int x, const int y, const int width, const int height);
    void set_scissor(const glm::ivec2 position, const glm::ivec2 size);
    void set_scissor(const common::rectangle& rect);

    void push_scissor(const common::rectangle& rect, ScissorMode mode);
    void replace_scissor(const common::rectangle& rect, ScissorMode mode);
    void pop_scissor();
    [[nodiscard]] const common::rectangle& peek_scissor();

    void apply_viewport(const viewport& viewport);
    [[nodiscard]] const viewport& get_viewport();

    [[nodiscard]] viewport letterbox_viewport(const viewport& to_fit, const int target_width, const int target_height, const bool fill);
}

