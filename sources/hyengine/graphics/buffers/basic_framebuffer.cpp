#include "basic_framebuffer.hpp"
#include <tracy/Tracy.hpp>
#include "../../core/logger.hpp"

namespace hyengine
{
    using namespace hyengine;


    basic_framebuffer::basic_framebuffer() : buffer(new frame_buffer()), depth_stencil_attachment(new texture_buffer()), color_attachment(new texture_buffer()), valid(false) {}

    basic_framebuffer::~basic_framebuffer()
    {
        free();

        delete depth_stencil_attachment;
        delete color_attachment;

        depth_stencil_attachment = nullptr;
        color_attachment = nullptr;
    }

    void basic_framebuffer::allocate(GLenum color_format, glm::uvec2 size, i32 multisample_count)
    {
        ZoneScoped;
        if (valid)
        {
            log_warn(stringify("Attempted to allocate already allocated standard framebuffer! (framebuffer ", buffer->get_id(), ")"), logger_tag);
            return;
        }

        buffer->allocate();
        depth_stencil_attachment->allocate(GL_TEXTURE_2D, {size.x, size.y, 0}, 1, GL_DEPTH24_STENCIL8, multisample_count);
        color_attachment->allocate(GL_TEXTURE_2D, {size.x, size.y, 0}, 1, color_format, multisample_count);

        buffer->attach_texture(*color_attachment, GL_COLOR_ATTACHMENT0);
        buffer->attach_texture(*depth_stencil_attachment, GL_DEPTH_STENCIL_ATTACHMENT);

        valid = buffer->validate();
        internal_format = color_format;

        log_info(stringify("Allocated standard framebuffer (framebuffer ", buffer->get_id(), ")"), logger_tag);
    }

    void basic_framebuffer::free()
    {
        ZoneScoped;
        if (valid)
        {
            const i32 prev_id = buffer->get_id();
            buffer->free();
            depth_stencil_attachment->free();
            color_attachment->free();
            valid = false;

            log_info(stringify("Freed standard framebuffer (framebuffer ", prev_id, ")"), logger_tag);
        }
    }

    void basic_framebuffer::clear_depth_stencil(const f32 depth, const i32 stencil) const
    {
        buffer->clear_depth_stencil_attachment(depth, stencil);
    }

    void basic_framebuffer::clear_color(const glm::vec4 color) const
    {
        buffer->clear_color_attachment(0, color);
    }

    void basic_framebuffer::clear_color(const glm::ivec4 data) const
    {
        buffer->clear_color_attachment(0, data);
    }

    void basic_framebuffer::clear_color(const glm::uvec4 data) const
    {
        buffer->clear_color_attachment(0, data);
    }


    void basic_framebuffer::bind_to_draw() const
    {
        buffer->bind_to_draw();
    }

    void basic_framebuffer::bind_to_read() const
    {
        buffer->bind_to_read();
    }
}
