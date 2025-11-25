#pragma once
#include "frame_buffer.hpp"

namespace hyengine
{
    using namespace hyengine;


    class basic_framebuffer
    {
    public:
        frame_buffer* buffer {};
        texture_buffer* depth_stencil_attachment {};
        texture_buffer* color_attachment {};

        basic_framebuffer(const basic_framebuffer& other) = delete;                //COPY CONSTRUCTOR
        basic_framebuffer(basic_framebuffer&& other) = delete;                     //MOVE CONSTRUCTOR
        basic_framebuffer& operator=(const basic_framebuffer& other) = delete;     //COPY ASSIGNMENT
        basic_framebuffer& operator=(basic_framebuffer&& other) noexcept = delete; //MOVE ASSIGNMENT


        explicit basic_framebuffer();
        ~basic_framebuffer();

        void allocate(const GLenum color_format, const glm::uvec2 size, const i32 multisample_count);
        void free();

        void clear_depth_stencil(const f32 depth, const i32 stencil) const;
        void clear_color(const glm::vec4 color) const;
        void clear_color(const glm::ivec4 data) const;
        void clear_color(const glm::uvec4 data) const;

        void bind_to_draw() const;
        void bind_to_read() const;

    private:
        const std::string logger_tag = "GPU Framebuffer";
        bool valid;
        GLenum internal_format = -1;
    };
}
