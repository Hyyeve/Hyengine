#include "frame_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"

namespace hyengine
{
    using namespace hyengine;


    frame_buffer::frame_buffer() = default;

    frame_buffer::~frame_buffer()
    {
        ZoneScoped;
        if (buffer_id > 0) free();
    }

    void frame_buffer::allocate()
    {
        ZoneScoped;
        if (buffer_id > 0)
        {
            log_warn(logger_tag, "Attempted to initialize already initialized framebuffer!", " (buffer ", buffer_id, ")");
            return;
        }

        glCreateFramebuffers(1, &buffer_id);

        is_valid = false;

        log_info(logger_tag, "Allocated framebuffer ", buffer_id, ".");
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding)
    {
        ZoneScoped;
        attach_texture(texture, attachment_binding, 0);
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level)
    {
        ZoneScoped;
        if (buffer_id <= 0)
        {
            log_warn(logger_tag, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferTexture(buffer_id, attachment_binding, texture.get_id(), mipmap_level);

        is_valid = false;
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level, const GLint layer)
    {
        ZoneScoped;
        if (buffer_id <= 0)
        {
            log_warn(logger_tag, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferTextureLayer(buffer_id, attachment_binding, texture.get_id(), mipmap_level, layer);

        is_valid = false;
    }

    void frame_buffer::attach_render_texture(const render_texture& render_texture, const GLenum attachment_binding)
    {
        ZoneScoped;
        if (buffer_id <= 0)
        {
            log_warn(logger_tag, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferRenderbuffer(buffer_id, attachment_binding, GL_RENDERBUFFER, render_texture.get_id());

        is_valid = false;
    }

    bool frame_buffer::validate()
    {
        ZoneScoped;
        if (is_valid)
        {
            return true;
        }

        if (buffer_id <= 0)
        {
            log_warn(logger_tag, "Framebuffer invalid ", buffer_id, " - not initialized!");
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

        log_warn(logger_tag, "Framebuffer ", buffer_id, " is not valid - ", status_string);
        return false;
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect) const
    {
        ZoneScoped;
        copy_data(source, rect, GL_LINEAR);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect, GLenum filter) const
    {
        ZoneScoped;
        copy_data(source, rect, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect, GLbitfield mask, GLenum filter) const
    {
        ZoneScoped;
        copy_data(source, rect, rect, mask, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 source_rect, glm::uvec4 dest_rect, GLbitfield mask, GLenum filter) const
    {
        ZoneScoped;
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tag, "Can't copy into framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBlitNamedFramebuffer(source.get_id(), buffer_id, source_rect.x, source_rect.y, source_rect.z, source_rect.w, // NOLINT(*-narrowing-conversions)
                               dest_rect.x, dest_rect.y, dest_rect.z, dest_rect.w, mask, filter); // NOLINT(*-narrowing-conversions)
    }

    void frame_buffer::free()
    {
        ZoneScoped;
        glDeleteFramebuffers(1, &buffer_id);
        log_info(logger_tag, "Freed framebuffer ", buffer_id, ".");
        buffer_id = 0;
        is_valid = false;
    }

    void frame_buffer::bind_to_draw() const
    {
        ZoneScoped;
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tag, "Can't bind framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer_id);
    }

    void frame_buffer::bind_to_read() const
    {
        ZoneScoped;
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tag, "Can't bind framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer_id);
    }

    GLuint frame_buffer::get_id() const
    {
        ZoneScoped;
        return buffer_id;
    }
}
