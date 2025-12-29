#include "frame_buffer.hpp"
#include <tracy/Tracy.hpp>
#include "../../core/logger.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    using namespace hyengine;

    frame_buffer::frame_buffer() = default;

    frame_buffer::~frame_buffer()
    {
        if (buffer_id > 0) free();
    }

    bool frame_buffer::allocate()
    {
        ZoneScoped;
        TracyGpuZone("allocate framebuffer");
        if (buffer_id > 0)
        {
            log_warn(logger_tags::GRAPHICS, "Attempted to initialize already initialized framebuffer!", " (buffer ", buffer_id, ")");
            return true;
        }

        glCreateFramebuffers(1, &buffer_id);
        is_valid = false;

        if (buffer_id <= 0)
        {
            log_error(logger_tags::GRAPHICS, "Failed to allocate framebuffer!");
            free();
            return false;
        }

        log_debug(logger_tags::GRAPHICS, "Allocated framebuffer ", buffer_id, ".");
        return true;
    }

    void frame_buffer::free()
    {
        ZoneScoped;
        TracyGpuZone("free framebuffer");
        glDeleteFramebuffers(1, &buffer_id);
        log_debug(logger_tags::GRAPHICS, "Freed framebuffer ", buffer_id, ".");
        buffer_id = 0;
        is_valid = false;
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding)
    {
        attach_texture(texture, attachment_binding, 0);
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level)
    {
        ZoneScoped;
        TracyGpuZone("attach framebuffer texture");
        if (buffer_id <= 0)
        {
            log_warn(logger_tags::GRAPHICS, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferTexture(buffer_id, attachment_binding, texture.get_id(), mipmap_level);

        is_valid = false;
    }

    void frame_buffer::attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level, const GLint layer)
    {
        ZoneScoped;
        TracyGpuZone("attach framebuffer texture");
        if (buffer_id <= 0)
        {
            log_warn(logger_tags::GRAPHICS, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferTextureLayer(buffer_id, attachment_binding, texture.get_id(), mipmap_level, layer);

        is_valid = false;
    }

    void frame_buffer::attach_render_texture(const render_texture& render_texture, const GLenum attachment_binding)
    {
        ZoneScoped;
        TracyGpuZone("attach framebuffer texture");
        if (buffer_id <= 0)
        {
            log_warn(logger_tags::GRAPHICS, "Can't attach texture to framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glNamedFramebufferRenderbuffer(buffer_id, attachment_binding, GL_RENDERBUFFER, render_texture.get_id());

        is_valid = false;
    }

    bool frame_buffer::validate()
    {
        ZoneScoped;
        TracyGpuZone("validate framebuffer");
        if (is_valid)
        {
            return true;
        }

        if (buffer_id <= 0)
        {
            log_warn(logger_tags::GRAPHICS, "Framebuffer invalid ", buffer_id, " - not initialized!");
            return false;
        }

        const GLenum status = glCheckNamedFramebufferStatus(buffer_id, GL_FRAMEBUFFER);

        if (status == GL_FRAMEBUFFER_COMPLETE)
        {
            log_debug(logger_tags::GRAPHICS, "Framebuffer ", buffer_id, " validated OK.");
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

        log_warn(logger_tags::GRAPHICS, "Framebuffer ", buffer_id, " is not valid - ", status_string);
        return false;
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect) const
    {
        copy_data(source, rect, GL_LINEAR);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect, GLenum filter) const
    {
        copy_data(source, rect, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 rect, GLbitfield mask, GLenum filter) const
    {
        copy_data(source, rect, rect, mask, filter);
    }

    void frame_buffer::copy_data(frame_buffer& source, glm::uvec4 source_rect, glm::uvec4 dest_rect, GLbitfield mask, GLenum filter) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer copy");
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tags::GRAPHICS, "Can't copy i32o framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBlitNamedFramebuffer(source.get_id(), buffer_id, source_rect.x, source_rect.y, source_rect.z, source_rect.w, // NOLINT(*-narrowing-conversions)
                               dest_rect.x, dest_rect.y, dest_rect.z, dest_rect.w, mask, filter);                      // NOLINT(*-narrowing-conversions)
    }

    void frame_buffer::bind_to_draw() const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer bind (draw)");
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tags::GRAPHICS, "Can't bind framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer_id);
    }

    void frame_buffer::bind_to_read() const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer bind (read)");
        if (buffer_id <= 0 || !is_valid)
        {
            log_warn(logger_tags::GRAPHICS, "Can't bind framebuffer ", buffer_id, " - not initialized!");
            return;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer_id);
    }

    void frame_buffer::clear_depth_stencil_attachment(const f32 depth, const i32 stencil) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (depth/stencil)");
        glClearNamedFramebufferfi(buffer_id, GL_DEPTH_STENCIL, 0, depth, stencil);
    }

    void frame_buffer::clear_depth_attachment(const f32 depth) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (depth)");
        glClearNamedFramebufferfv(buffer_id, GL_DEPTH, 0, &depth);
    }

    void frame_buffer::clear_stencil_attachment(const i32 stencil) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (stencil)");
        glClearNamedFramebufferiv(buffer_id, GL_STENCIL, 0, &stencil);
    }

    void frame_buffer::clear_color_attachment(const u8 drawbuffer_index, const glm::vec4 color) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (color)");
        glClearNamedFramebufferfv(buffer_id, GL_COLOR, drawbuffer_index, glm::value_ptr(color));
    }

    void frame_buffer::clear_color_attachment(const u8 drawbuffer_index, const glm::ivec4 data) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (color)");
        glClearNamedFramebufferiv(buffer_id, GL_COLOR, drawbuffer_index, glm::value_ptr(data));
    }

    void frame_buffer::clear_color_attachment(const u8 drawbuffer_index, const glm::uvec4 data) const
    {
        ZoneScoped;
        TracyGpuZone("framebuffer clear (color)");
        glClearNamedFramebufferuiv(buffer_id, GL_COLOR, drawbuffer_index, glm::value_ptr(data));
    }

    GLuint frame_buffer::get_id() const
    {
        return buffer_id;
    }
}
