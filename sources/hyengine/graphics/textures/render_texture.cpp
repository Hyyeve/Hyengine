#include "render_texture.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"
#include "hyengine/common/common.hpp"
#include "hyengine/graphics/gl_enums.hpp"

namespace hyengine
{
    render_texture::~render_texture()
    {
        if (gl_id > 0)
        {
            free();
        }
    }

    void render_texture::allocate(const GLenum format, const glm::uvec2 size, const i32 multisample_count)
    {
        ZoneScoped;
        if (gl_id > 0)
        {
            log_warn(logger_tag, "Attempted to initialize already initialized buffer!", " (buffer ", gl_id, ")");
            return;
        }

        internal_size = size;
        internal_samples = multisample_count;
        internal_format = format;

        glCreateRenderbuffers(1, &gl_id);
        glNamedRenderbufferStorageMultisample(gl_id, internal_samples, internal_format, internal_size.x, internal_size.y);

        log_info(logger_tag, "Allocated render texture ", gl_id, ": ", internal_size.x, "x", internal_size.y, " ", stringify_count(internal_samples, "MSAA sample"));
    }

    void render_texture::free()
    {
        ZoneScoped;
        glDeleteRenderbuffers(1, &gl_id);
        log_info(logger_tag, "Freed render texture ", gl_id, ".");
        gl_id = 0;
    }

    void render_texture::copy_data_from(const render_texture& source) const
    {
        copy_data_from(source.get_id());
    }

    void render_texture::copy_data_from(const render_texture& source, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const
    {
        copy_data_from(source.get_id(), from_pos, to_pos, size);
    }

    void render_texture::copy_data_from(const GLuint source) const
    {
        copy_data_from(source, {0, 0}, {0, 0}, internal_size);
    }

    void render_texture::copy_data_from(const GLuint source, const glm::uvec2 from_pos, const glm::uvec2 to_pos, const glm::uvec2 size) const
    {
        copy_render_texture_data(source, gl_id, from_pos, to_pos, size);
    }

    void render_texture::copy_data_to(const GLuint dest) const
    {
        copy_data_to(dest, {0, 0}, {0, 0}, internal_size);
    }

    void render_texture::copy_data_to(const GLuint dest, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const
    {
        copy_render_texture_data(gl_id, dest, from_pos, to_pos, size);
    }

    void render_texture::copy_render_texture_data(const GLuint source, const GLuint dest, const glm::uvec2 source_pos, const glm::uvec2 dest_pos, const glm::uvec2 size)
    {
        ZoneScoped;
        glCopyImageSubData(source, GL_RENDERBUFFER, 0, source_pos.x, source_pos.y, 0, dest, GL_RENDERBUFFER, 0, dest_pos.x, dest_pos.y, 0, size.x, size.y, 0);
    }

    GLuint render_texture::get_id() const
    {
        return gl_id;
    }

    glm::uvec2 render_texture::get_size() const
    {
        return internal_size;
    }

    GLenum render_texture::get_format() const
    {
        return internal_format;
    }

    i32 render_texture::get_multisample_count() const
    {
        return internal_samples;
    }

    bool render_texture::is_signed_integer_format() const
    {
        return texture_formats::is_signed_integer(internal_format);
    }

    bool render_texture::is_unsigned_integer_format() const
    {
        return texture_formats::is_unsigned_integer(internal_format);
    }

    bool render_texture::is_float_format() const
    {
        return texture_formats::is_floating_point(internal_format);
    }
}
