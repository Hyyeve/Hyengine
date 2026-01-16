#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backend_impl_gl.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace stardraw
{
    inline GLenum gl_draw_mode(const draw_mode mode)
    {
        switch (mode)
        {
            case draw_mode::TRIANGLES: return GL_TRIANGLES;
            case draw_mode::TRIANGLE_FAN: return GL_TRIANGLE_FAN;
            case draw_mode::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
        }

        return -1;
    }

    inline uint32_t gl_type_size(const GLenum type)
    {
        switch (type)
        {
            case GL_INT: return sizeof(GLint);
            case GL_SHORT: return sizeof(GLshort);
            case GL_BYTE: return sizeof(GLbyte);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_SHORT: return sizeof(GLushort);
            case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_DOUBLE: return sizeof(GLdouble);
            default: return -1;
        }
    }

    status backend_impl_gl::execute_draw_cmd(const draw_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute draw cmd");

        const status bind_result = bind_vertex_buffer(cmd->vertex_source);
        if (bind_result != status::SUCCESS) return bind_result;
        glDrawArraysInstancedBaseInstance(gl_draw_mode(cmd->mode), cmd->vertex_source_offset, cmd->count, cmd->instances, cmd->instance_offset);
        return status::SUCCESS;
    }

    status backend_impl_gl::execute_draw_indexed(const draw_indexed_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute draw indexed cmd");

        const status vertex_bind_result = bind_vertex_buffer(cmd->vertex_source);
        if (vertex_bind_result != status::SUCCESS) return vertex_bind_result;
        const status index_bind_result = bind_index_buffer(cmd->index_source);
        if (index_bind_result != status::SUCCESS) return index_bind_result;

        const GLenum index_element_type = GL_UNSIGNED_INT; //TODO: Read from index buffer state
        const uint32_t index_element_size = gl_type_size(index_element_type);

        glDrawElementsInstancedBaseVertexBaseInstance(gl_draw_mode(cmd->mode), cmd->count, index_element_type, reinterpret_cast<const void*>(cmd->index_source_offset * index_element_size), cmd->instances, cmd->vertex_source_offset, cmd->instance_offset);

        return status::SUCCESS;
    }

    status backend_impl_gl::execute_draw_indirect(const draw_indirect_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute draw indirect cmd");

        const status vertex_bind_result = bind_vertex_buffer(cmd->vertex_source);
        if (vertex_bind_result != status::SUCCESS) return vertex_bind_result;
        const status indirect_bind_result = bind_indirect_buffer(cmd->indirect_source);
        if (indirect_bind_result != status::SUCCESS) return indirect_bind_result;

        glMultiDrawArraysIndirect(gl_draw_mode(cmd->mode), reinterpret_cast<const void*>(cmd->indirect_source_offset * sizeof(gl_draw_arrays_indirect_params)), cmd->draw_count, 0);
        return status::SUCCESS;
    }

    status backend_impl_gl::execute_draw_indexed_indirect(const draw_indexed_indirect_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute draw indirect cmd");

        const status vertex_bind_result = bind_vertex_buffer(cmd->vertex_source);
        if (vertex_bind_result != status::SUCCESS) return vertex_bind_result;
        const status index_bind_result = bind_index_buffer(cmd->index_source);
        if (index_bind_result != status::SUCCESS) return index_bind_result;
        const status indirect_bind_result = bind_indirect_buffer(cmd->indirect_source);
        if (indirect_bind_result != status::SUCCESS) return indirect_bind_result;

        const GLenum index_element_type = GL_UNSIGNED_INT; //TODO: Read from index buffer state

        glMultiDrawElementsIndirect(gl_draw_mode(cmd->mode), index_element_type, reinterpret_cast<const void*>(cmd->indirect_source_offset * sizeof(gl_draw_elements_indirect_params)), cmd->draw_count, 0);
        return status::SUCCESS;
    }

    inline GLenum gl_blend_factor(const blending_factor factor)
    {
        switch (factor)
        {
            case blending_factor::ZERO: return GL_ZERO;
            case blending_factor::ONE: return GL_ONE;
            case blending_factor::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
            case blending_factor::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
            case blending_factor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
            case blending_factor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
            case blending_factor::SOURCE_COLOR: return GL_SRC_COLOR;
            case blending_factor::DEST_COLOR: return GL_DST_COLOR;
            case blending_factor::ONE_MINUS_SOURCE_COLOR: return GL_ONE_MINUS_SRC_COLOR;
            case blending_factor::ONE_MINUS_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
            case blending_factor::SOURCE_ALPHA: return GL_SRC_ALPHA;
            case blending_factor::DEST_ALPHA: return GL_DST_ALPHA;
            case blending_factor::ONE_MINUS_SOURCE_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
            case blending_factor::ONE_MINUS_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
            case blending_factor::SOURCE_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
            case blending_factor::SECONDARY_SOURCE_COLOR: return GL_SRC1_COLOR;
            case blending_factor::SECONDARY_SOURCE_ALPHA: return GL_SRC1_ALPHA;
        }
        return -1;
    }

    inline GLenum gl_blend_func(const blending_func func)
    {
        switch (func)
        {
            case blending_func::ADD: return GL_FUNC_ADD;
            case blending_func::SUBTRACT: return GL_FUNC_SUBTRACT;
            case blending_func::REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
            case blending_func::MIN: return GL_MIN;
            case blending_func::MAX: return GL_MAX;
        }

        return -1;
    }

    status backend_impl_gl::execute_config_blending(const config_blending_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute config blending cmd");
        const blending_config& config = cmd->config;
        glBlendColor(config.constant_blend_r, config.constant_blend_g, config.constant_blend_b, config.constant_blend_a);
        glBlendEquationSeparatei(cmd->draw_buffer_index, gl_blend_func(config.rgb_equation), gl_blend_func(config.alpha_equation));
        glBlendFuncSeparatei(cmd->draw_buffer_index, gl_blend_factor(config.source_blend_rgb), gl_blend_factor(config.dest_blend_rgb), gl_blend_factor(config.source_blend_alpha), gl_blend_factor(config.dest_blend_alpha));
        return status::SUCCESS;
    }

    inline GLenum gl_stencil_facing(const triangle_facing facing)
    {
        switch (facing)
        {
            case triangle_facing::FRONT: return GL_FRONT;
            case triangle_facing::BACK: return GL_BACK;
            case triangle_facing::BOTH: return GL_FRONT_AND_BACK;
        }

        return -1;
    }

    inline GLenum gl_stencil_test_func(const stencil_test_func test_func)
    {
        switch (test_func)
        {
            case stencil_test_func::ALWAYS: return GL_ALWAYS;
            case stencil_test_func::NEVER: return GL_NEVER;
            case stencil_test_func::LESS: return GL_LESS;
            case stencil_test_func::LESS_EQUAL: return GL_LEQUAL;
            case stencil_test_func::GREATER: return GL_GREATER;
            case stencil_test_func::GREATER_EQUAL: return GL_GEQUAL;
            case stencil_test_func::EQUAL: return GL_EQUAL;
            case stencil_test_func::NOT_EQUAL: return GL_NOTEQUAL;
        }
        return -1;
    }

    inline GLenum gl_stencil_test_op(const stencil_result_op stencil_op)
    {
        switch (stencil_op)
        {
            case stencil_result_op::KEEP: return GL_KEEP;
            case stencil_result_op::ZERO: return GL_ZERO;
            case stencil_result_op::REPLACE: return GL_REPLACE;
            case stencil_result_op::INCREMENT: return GL_INCR;
            case stencil_result_op::INCREMENT_WRAP: return GL_INCR_WRAP;
            case stencil_result_op::DECREMENT: return GL_DECR;
            case stencil_result_op::DECREMENT_WRAP: return GL_DECR_WRAP;
            case stencil_result_op::INVERT: return GL_INVERT;
        }
        return -1;
    }

    status backend_impl_gl::execute_config_stencil(const config_stencil_command* cmd)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Execute config stencil cmd");
        const stencil_config& config = cmd->config;
        const GLenum gl_facing = gl_stencil_facing(cmd->for_facing);
        glStencilFuncSeparate(gl_facing, gl_stencil_test_func(config.test_func), config.reference, config.test_mask);
        glStencilMaskSeparate(gl_facing, config.write_mask);
        glStencilOpSeparate(gl_facing, gl_stencil_test_op(config.stencil_fail_op), gl_stencil_test_op(config.depth_fail_op), gl_stencil_test_op(config.pixel_pass_op));
        return status::SUCCESS;
    }
}
