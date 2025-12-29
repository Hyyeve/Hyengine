#pragma once
#include "frame_buffer.hpp"
#include "hyengine/core/logger.hpp"
#include "hyengine/graphics/gl_enums.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    using namespace hyengine;

    ///Standard wrapper to create a framebuffer with a number of color attachments and a depth/stencil attachment. \n
    ///Attachments are all created as textures (potentially slower to render, but can be sampled from other passes) - if you want renderbuffers, you need to setup a frame_buffer manually.
    template <u32 num_color_buffers>
    class standard_framebuffer
    {
    public:
        struct color_attachment_info
        {
            color_attachment_info(const GLenum internal_format, const i32 mipmaps) : format(internal_format), mipmap_count(mipmaps) {};
            // ReSharper disable once CppNonExplicitConvertingConstructor
            color_attachment_info(const GLenum internal_format) : color_attachment_info(internal_format, 1) {};

            GLenum format;
            i32 mipmap_count;
        };

        struct depth_attachment_info
        {
            // ReSharper disable once CppNonExplicitConvertingConstructor
            depth_attachment_info(const GLenum internal_format) : format(internal_format), has_depth_attachment(true) {};
            depth_attachment_info() : format(0), has_depth_attachment(false) {};

            GLenum format;
            bool has_depth_attachment;
        };

        frame_buffer framebuffer = frame_buffer();
        std::array<texture_buffer, num_color_buffers> color_buffers;
        texture_buffer depth_stencil_attachment = texture_buffer();

        standard_framebuffer(const standard_framebuffer& other) = delete;                //COPY CONSTRUCTOR
        standard_framebuffer(standard_framebuffer&& other) = delete;                     //MOVE CONSTRUCTOR
        standard_framebuffer& operator=(const standard_framebuffer& other) = delete;     //COPY ASSIGNMENT
        standard_framebuffer& operator=(standard_framebuffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit standard_framebuffer() noexcept = default;
        ~standard_framebuffer()
        {
            free();
        }

        [[nodiscard]] bool allocate(const glm::uvec2 size, const std::array<color_attachment_info, num_color_buffers> color_attachments, const depth_attachment_info depth_attachment_info, const i32 multisample_count)
        {
            ZoneScoped;
            TracyGpuZone("allocate basic framebuffer");
            if (valid)
            {
                log_warn(logger_tags::GRAPHICS, stringify("Attempted to allocate already allocated basic framebuffer! (framebuffer ", framebuffer.get_id(), ")"));
                return true;
            }

            const bool buffer_allocated = framebuffer.allocate();
            bool color_attachments_allocated = true;

            for (u32 i = 0; i < num_color_buffers; i++)
            {
                color_attachment_info info = color_attachments[i];
                const bool allocated_okay = color_buffers[i].allocate(GL_TEXTURE_2D, {size.x, size.y, 0}, info.mipmap_count, info.format, multisample_count);
                color_attachments_allocated &= allocated_okay;
            }

            bool depth_allocated = true;
            if (depth_attachment_info.has_depth_attachment)
            {
                depth_allocated = depth_stencil_attachment.allocate(GL_TEXTURE_2D, {size.x, size.y, 0}, 1, depth_attachment_info.format, multisample_count);
            }

            if (!buffer_allocated || !depth_allocated || !color_attachments_allocated)
            {
                log_error(logger_tags::GRAPHICS, "Failed to allocate basic framebuffer!");
                free();
                return false;
            }

            for (u32 i = 0; i < num_color_buffers; i++)
            {
                framebuffer.attach_texture(color_buffers[i], GL_COLOR_ATTACHMENT0 + i);
            }

            if (depth_attachment_info.has_depth_attachment)
            {
                GLenum attachment_binding = GL_DEPTH_STENCIL_ATTACHMENT;
                if (depth_stencil_formats::is_depth_only(depth_attachment_info.format)) attachment_binding = GL_DEPTH_ATTACHMENT;
                else if (depth_stencil_formats::is_stencil_only(depth_attachment_info.format)) attachment_binding = GL_STENCIL_ATTACHMENT;
                framebuffer.attach_texture(depth_stencil_attachment, attachment_binding);
            }

            valid = framebuffer.validate();

            if (!valid)
            {
                log_error(logger_tags::GRAPHICS, "Failed to allocate basic framebuffer!");
                free();
                return false;
            }

            log_debug(logger_tags::GRAPHICS, "Buffer ", framebuffer.get_id(), " allocated as basic framebuffer.");
            return true;
        }

        void free()
        {
            ZoneScoped;
            TracyGpuZone("free basic framebuffer");
            if (valid)
            {
                const i32 prev_id = framebuffer.get_id();
                framebuffer.free();
                depth_stencil_attachment.free();
                for (u32 index = 0; index < num_color_buffers; index++)
                {
                    color_buffers[index].free();
                }

                valid = false;

                log_debug(logger_tags::GRAPHICS, stringify("Freed standard framebuffer (framebuffer ", prev_id, ")"));
            }
        }

        void copy_data(frame_buffer& source, glm::uvec4 rect) const
        {
            framebuffer.copy_data(source, rect);
        }

        void copy_data(frame_buffer& source, glm::uvec4 rect, GLenum filter) const
        {
            framebuffer.copy_data(source, rect, filter);
        }

        void copy_data(frame_buffer& source, glm::uvec4 rect, GLbitfield mask, GLenum filter) const
        {
            framebuffer.copy_data(source, rect, mask, filter);
        }

        void copy_data(frame_buffer& source, glm::uvec4 source_rect, glm::uvec4 dest_rect, GLbitfield mask, GLenum filter) const
        {
            framebuffer.copy_data(source, source_rect, dest_rect, mask, filter);
        }

        void bind_to_draw() const
        {
            framebuffer.bind_to_draw();
        }

        void bind_to_read() const
        {
            framebuffer.bind_to_read();
        }

        void clear_depth_stencil(const f32 depth, const i32 stencil) const
        {
            framebuffer.clear_depth_stencil_attachment(depth, stencil);
        }

        void clear_depth(const f32 depth) const
        {
            framebuffer.clear_depth_attachment(depth);
        }

        void clear_stencil(const i32 stencil) const
        {
            framebuffer.clear_stencil_attachment(stencil);
        }

        void clear_color(const u8 drawbuffer_index, const glm::vec4 color) const
        {
            framebuffer.clear_color_attachment(drawbuffer_index, color);
        }

        void clear_color(const u8 drawbuffer_index, const glm::ivec4 data) const
        {
            framebuffer.clear_color_attachment(drawbuffer_index, data);
        }

        void clear_color(const u8 drawbuffer_index, const glm::uvec4 data) const
        {
            framebuffer.clear_color_attachment(drawbuffer_index, data);
        }

    private:
        bool valid = false;
    };
}
