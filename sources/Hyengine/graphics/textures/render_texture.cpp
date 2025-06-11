#include "render_texture.hpp"

#include "../../core/logger.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    render_texture::~render_texture()
    {
        if (gl_id > 0)
        {
            free();
        }
    }

    void render_texture::allocate(const GLenum format, const glm::uvec2 size, const int multisample_count)
    {
        if(gl_id > 0) {
            logger::message_warn(logger::format("Attempted to initialize already initialized buffer!", " (buffer ", gl_id, ")"), logger_tag);
            return;
        }

        internal_size = size;
        internal_samples = multisample_count;
        internal_format = format;

        glCreateRenderbuffers(1, &gl_id);
        glNamedRenderbufferStorageMultisample(gl_id, internal_samples, internal_format, internal_size.x, internal_size.y);

        logger::message_info(logger::format("Allocated render texture ", gl_id, ": ", internal_size.x, "x", internal_size.y, " ", logger::format_count(internal_samples, "MSAA sample")), logger_tag);
    }

    void render_texture::free()
    {
        glDeleteRenderbuffers(1, &gl_id);
        logger::message_info(logger::format("Freed render texture ", gl_id, "."), logger_tag);
        gl_id = 0;
    }

    void render_texture::copy_data(const render_texture& source) const
    {
        copy_data_partial(source, {0, 0}, {0, 0}, {internal_size.x, internal_size.y});
    }

    void render_texture::copy_data_partial(const render_texture& source, const glm::uvec2 from_pos, const glm::uvec2 to_pos, const glm::uvec2 size) const
    {
        glCopyImageSubData(source.get_id(), GL_RENDERBUFFER, 0, from_pos.x, from_pos.y, 0, gl_id, GL_RENDERBUFFER, 0, to_pos.x, to_pos.y, 0, size.x, size.y, 0);
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

    int render_texture::get_multisample_count() const
    {
        return internal_samples;
    }
}
