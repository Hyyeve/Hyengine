#pragma once

#include <string>

#include "../vertex_formats.hpp"
#include "../../library/gl.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    class vertex_format_buffer
    {
    public:

        vertex_format_buffer(const vertex_format_buffer& other) = delete; //COPY CONSTRUCTOR
        vertex_format_buffer(vertex_format_buffer&& other) = delete; //MOVE CONSTRUCTOR
        vertex_format_buffer& operator=(const vertex_format_buffer& other) = delete; //COPY ASSIGNMENT
        vertex_format_buffer& operator=(vertex_format_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit vertex_format_buffer();
        ~vertex_format_buffer();

        void allocate();
        void free();
        void bind_state() const;

        void attach_vertex_buffer(const GLuint binding_slot, const GLuint vertex_buffer_id, const GLintptr vertex_buffer_offset, const GLsizei vertex_buffer_stride) const;
        void attach_index_buffer(const GLuint index_buffer_id) const;

        template <size_t format_size>
        void attach_vertex_format(const vertex_format<format_size>& format, const GLuint binding_slot) const
        {
            format.apply(buffer_id, binding_slot);
        }

        static void unbind_state();
        [[nodiscard]] GLuint get_id() const;



    private:
        const std::string logger_tag = "GPU Vertex Format Buffer";
        GLuint buffer_id = 0;
    };
}