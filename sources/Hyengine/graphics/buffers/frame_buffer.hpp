#pragma once
#include "../textures/render_texture.hpp"
#include "../textures/texture_buffer.hpp"
#include "../../library/gl.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    using namespace hyengine;


    class frame_buffer
    {
    public:
        frame_buffer(const frame_buffer& other) = delete; //COPY CONSTRUCTOR
        frame_buffer(frame_buffer&& other) = delete; //MOVE CONSTRUCTOR
        frame_buffer& operator=(const frame_buffer& other) = delete; //COPY ASSIGNMENT
        frame_buffer& operator=(frame_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit frame_buffer();
        ~frame_buffer();

        void allocate();
        void free();

        void attach_texture(const texture_buffer& texture, const GLenum attachment_binding);
        void attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level);
        void attach_texture(const texture_buffer& texture, const GLenum attachment_binding, const GLint mipmap_level, const GLint layer);
        void attach_render_texture(const render_texture& render_texture, const GLenum attachment_binding);

        bool validate();

        void copy_data(frame_buffer& source, glm::uvec4 rect) const;
        void copy_data(frame_buffer& source, glm::uvec4 rect, GLenum filter) const;
        void copy_data(frame_buffer& source, glm::uvec4 rect, GLbitfield mask, GLenum filter) const;
        void copy_data(frame_buffer& source, glm::uvec4 source_rect, glm::uvec4 dest_rect, GLbitfield mask, GLenum filter) const;

        void bind_to_draw() const;
        void bind_to_read() const;

        [[nodiscard]] GLuint get_id() const;

    private:
        const std::string logger_tag = "GPU Framebuffer";
        GLuint buffer_id = 0;
        bool is_valid = false;
    };
}
