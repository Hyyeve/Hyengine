#include "vertex_format_buffer.hpp"

#include "../../core/logger.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    vertex_format_buffer::vertex_format_buffer() = default;

    void vertex_format_buffer::bind_state() const
    {
        glBindVertexArray(buffer_id);
    }

    void vertex_format_buffer::attach_vertex_buffer(const GLuint binding_slot, const GLuint vertex_buffer_id, const GLintptr vertex_buffer_offset, const GLsizei vertex_buffer_stride) const
    {
        glVertexArrayVertexBuffer(buffer_id, binding_slot, vertex_buffer_id, vertex_buffer_offset, vertex_buffer_stride);
    }

    void vertex_format_buffer::attach_index_buffer(const GLuint index_buffer_id) const
    {
        glVertexArrayElementBuffer(buffer_id, index_buffer_id);
    }

    void vertex_format_buffer::unbind_state()
    {
        glBindVertexArray(0);
    }

    GLuint vertex_format_buffer::get_id() const
    {
        return buffer_id;
    }

    vertex_format_buffer::~vertex_format_buffer()
    {
        free();
    }

    void vertex_format_buffer::allocate()
    {
        if (buffer_id > 0)
        {
            logger::message_warn(logger::format("Attempted to initialize already initialized format buffer!", " (buffer ", buffer_id, ")"), logger_tag);
        }
        glCreateVertexArrays(1, &buffer_id);
    }

    void vertex_format_buffer::free()
    {
        if (buffer_id > 0)
        {
            glDeleteVertexArrays(1, &buffer_id);
            buffer_id = 0;
        }
    }
}
