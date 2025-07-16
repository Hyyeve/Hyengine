#pragma once

#include "texture_sampler_state.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine::graphics {
    texture_sampler_state::~texture_sampler_state()
    {
        ZoneScoped;
        free();
    }

    void texture_sampler_state::allocate()
    {
        ZoneScoped;
        glCreateSamplers(1, &gl_id);
    }

    void texture_sampler_state::free()
    {
        ZoneScoped;
        glDeleteSamplers(1, &gl_id);
        gl_id = -1;
    }

    void texture_sampler_state::set_border_color(const glm::vec4 col) const
    {
        ZoneScoped;
        glSamplerParameterfv(gl_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }

    void texture_sampler_state::set_compare_func(const GLenum func) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_COMPARE_FUNC, func);
    }

    void texture_sampler_state::set_compare_mode(const GLenum mode) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_COMPARE_MODE, mode);
    }

    void texture_sampler_state::set_anisotropy_level(const float anisotropy) const
    {
        ZoneScoped;
        if (GLAD_GL_VERSION_4_6 || GLAD_GL_ARB_texture_filter_anisotropic || GLAD_GL_EXT_texture_filter_anisotropic)
        {
            glSamplerParameterf(gl_id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
        }
    }

    void texture_sampler_state::set_downscale_filter(const GLenum filter) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_MIN_FILTER, filter);
    }

    void texture_sampler_state::set_upscale_filter(const GLenum filter) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_MAG_FILTER, filter);
    }

    void texture_sampler_state::set_lod_bias(const float bias) const
    {
        ZoneScoped;
        glSamplerParameterf(gl_id, GL_TEXTURE_LOD_BIAS, bias);
    }

    void texture_sampler_state::set_min_lod(const int lod) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_MIN_LOD, lod);
    }

    void texture_sampler_state::set_max_lod(const int lod) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_MAX_LOD, lod);
    }

    void texture_sampler_state::set_texture_wrap_x(const GLenum wrap) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_WRAP_S, wrap);
    }

    void texture_sampler_state::set_texture_wrap_y(const GLenum wrap) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_WRAP_T, wrap);
    }

    void texture_sampler_state::set_texture_wrap_z(const GLenum wrap) const
    {
        ZoneScoped;
        glSamplerParameteri(gl_id, GL_TEXTURE_WRAP_R, wrap);
    }

    void texture_sampler_state::bind(const int slot) const
    {
        ZoneScoped;
        glBindSampler(slot, gl_id);
    }

    GLuint texture_sampler_state::get_id() const
    {
        ZoneScoped;
        return gl_id;
    }
}
