#include "basic_framebuffer.hpp"

#include "../../core/logger.hpp"

namespace hyengine::graphics {

    using namespace hyengine;


    basic_framebuffer::basic_framebuffer():
    buffer(new frame_buffer()), depth_stencil_attachment(new render_texture()), color_attachment(new texture_buffer()), valid(false)
    {}

    basic_framebuffer::~basic_framebuffer()
    {
        free();

        delete depth_stencil_attachment;
        delete color_attachment;

        depth_stencil_attachment = nullptr;
        color_attachment = nullptr;
    }

    void basic_framebuffer::allocate(GLenum color_format, glm::uvec2 size, int multisample_count)
    {
        if(valid) {
            logger::message_warn(logger::format("Attempted to allocate already allocated standard framebuffer! (framebuffer ", buffer->get_id(), ")"), logger_tag);
            return;
        }

        buffer->allocate();
        depth_stencil_attachment->allocate(GL_DEPTH24_STENCIL8, size, multisample_count);
        color_attachment->allocate(GL_TEXTURE_2D, {size.x, size.y, 0}, 1, color_format, multisample_count);

        buffer->attach_texture(*color_attachment, GL_COLOR_ATTACHMENT0);
        buffer->attach_render_texture(*depth_stencil_attachment, GL_DEPTH_STENCIL_ATTACHMENT);

        valid = buffer->validate();

        logger::message_info(logger::format("Allocated standard framebuffer (framebuffer ", buffer->get_id(), ")"), logger_tag);
    }

    void basic_framebuffer::free()
    {
        if(valid)
        {
            const int prev_id = buffer->get_id();
            buffer->free();
            depth_stencil_attachment->free();
            color_attachment->free();
            valid = false;

            logger::message_info(logger::format("Freed standard framebuffer (framebuffer ", prev_id, ")"), logger_tag);
        }
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
