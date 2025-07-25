#pragma once
#include "frame_buffer.hpp"

namespace hyengine
{
    using namespace hyengine;


    class basic_framebuffer
    {
    public:
        frame_buffer* buffer{};
        render_texture* depth_stencil_attachment{};
        texture_buffer* color_attachment{};

        basic_framebuffer(const basic_framebuffer& other) = delete; //COPY CONSTRUCTOR
        basic_framebuffer(basic_framebuffer&& other) = delete; //MOVE CONSTRUCTOR
        basic_framebuffer& operator=(const basic_framebuffer& other) = delete; //COPY ASSIGNMENT
        basic_framebuffer& operator=(basic_framebuffer&& other) noexcept = delete; //MOVE ASSIGNMENT


        explicit basic_framebuffer();
        ~basic_framebuffer();

        void allocate(GLenum color_format, glm::uvec2 size, int multisample_count);
        void free();

        void bind_to_draw() const;
        void bind_to_read() const;

    private:
        const std::string logger_tag = "GPU Framebuffer";
        bool valid;
    };
}
