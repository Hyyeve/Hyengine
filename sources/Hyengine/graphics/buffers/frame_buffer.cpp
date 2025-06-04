#include "frame_buffer.hpp"

#include "../../core/logger.hpp"

namespace hyengine::graphics {

    using namespace hyengine;


    frame_buffer::frame_buffer() = default;

    frame_buffer::~frame_buffer()
    {
        if (buffer_id > 0) free();
    }

    void frame_buffer::allocate()
    {
        if(buffer_id > 0) {
            logger::message_warn(logger::format("Attempted to initialize already initialized framebuffer!", " (buffer ", buffer_id, ")"), logger_tag);
            return;
        }

        glCreateFramebuffers(1, &buffer_id);

        is_valid = false;

        logger::message_info(logger::format("Allocated framebuffer ", buffer_id, "."), logger_tag);
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding)
    {
        attach_texture(texture, attachment_binding, 0);
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level)
    {
        if(buffer_id <= 0) {
            logger::message_warn(logger::format("Can't attach texture to framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glNamedFramebufferTexture(buffer_id, attachment_binding, texture.get_id(), mipmap_level);

        is_valid = false;
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level, const GLint layer)
    {
        if(buffer_id <= 0) {
            logger::message_warn(logger::format("Can't attach texture to framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glNamedFramebufferTextureLayer(buffer_id, attachment_binding, texture.get_id(), mipmap_level, layer);

        is_valid = false;
    }

    void frame_buffer::attach_render_texture(const render_texture& render_texture, const GLenum attachment_binding)
    {
        if(buffer_id <= 0) {
            logger::message_warn(logger::format("Can't attach texture to framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glNamedFramebufferRenderbuffer(buffer_id, attachment_binding, GL_RENDERBUFFER, render_texture.get_id());

        is_valid = false;
    }

    bool frame_buffer::validate()
    {
        if (is_valid)
        {
            return true;
        }

        if(buffer_id <= 0) {
            logger::message_warn(logger::format("Framebuffer invalid ", buffer_id, " - not initialized!"), logger_tag);
            return false;
        }

        const GLenum status = glCheckNamedFramebufferStatus(buffer_id, GL_FRAMEBUFFER);

        if (status == GL_FRAMEBUFFER_COMPLETE)
        {
            is_valid = true;
            return true;
        }

        std::string status_string;

        switch (status)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
            {
                status_string = "Invalid framebuffer object";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            {
                status_string = "Incomplete framebuffer attachments";
                break;
            }
            case GL_FRAMEBUFFER_UNSUPPORTED:
            {
                status_string = "Unsupported framebuffer format";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            {
                status_string = "Attachments have mismatched multisample settings";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            {
                status_string = "Attachments have mismatched layer settings";
                break;
            }
            default:
            {
                status_string = "Unknown reason";
                break;
            }
        }

        logger::message_warn(logger::format("Framebuffer ", buffer_id, " is not valid - ", status_string), logger_tag);
        return false;
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec2 pos, glm::uvec2 size) const
    {
        copy_data(source, pos, size, GL_LINEAR);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec2 pos, glm::uvec2 size, GLenum filter) const
    {
        copy_data(source, pos, size, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec2 pos, glm::uvec2 size, GLbitfield mask, GLenum filter) const
    {
        copy_data(source, pos, size, pos, size, mask, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec2 source_pos, glm::uvec2 source_size, glm::uvec2 to_pos, glm::uvec2 to_size, GLbitfield mask, GLenum filter) const
    {
        if(buffer_id <= 0 || !is_valid)
        {
            logger::message_warn(logger::format("Can't copy into framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glBlitNamedFramebuffer(source.get_id(), buffer_id, source_pos.x, source_pos.y, source_pos.x + source_size.x, source_pos.y + source_size.y,
                               to_pos.x, to_pos.y, to_pos.x + to_size.x, to_pos.y + to_size.y, mask, filter);
    }

    void frame_buffer::free()
    {
        glDeleteFramebuffers(1, &buffer_id);
        logger::message_info(logger::format("Freed framebuffer ", buffer_id, "."), logger_tag);
        buffer_id = 0;
        is_valid = false;
    }

    void frame_buffer::bind_to_draw() const
    {
        if(buffer_id <= 0 || !is_valid)
        {
            logger::message_warn(logger::format("Can't bind framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer_id);
    }

    void frame_buffer::bind_to_read() const
    {
        if(buffer_id <= 0 || !is_valid)
        {
            logger::message_warn(logger::format("Can't bind framebuffer ", buffer_id, " - not initialized!"), logger_tag);
            return;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer_id);
    }

    GLuint frame_buffer::get_id() const
    {
        return buffer_id;
    }
}
