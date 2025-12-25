#include "vertex_format_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    vertex_format_buffer::vertex_format_buffer() = default;

    vertex_format_buffer::~vertex_format_buffer()
    {
        free();
    }

    bool vertex_format_buffer::allocate()
    {
        ZoneScoped;
        TracyGpuZone("allocate vertex format buffer");
        if (buffer_id > 0)
        {
            log_warn(logger_tags::GRAPHICS, "Attempted to initialize already initialized format buffer!", " (buffer ", buffer_id, ")");
            return true;
        }

        glCreateVertexArrays(1, &buffer_id);
        if (buffer_id == 0)
        {
            log_error(logger_tags::GRAPHICS, "Failed to allocate vertex format buffer!");
            return false;
        }

        return true;
    }

    void vertex_format_buffer::free()
    {
        ZoneScoped;
        TracyGpuZone("free vertex format buffer");
        if (buffer_id > 0)
        {
            glDeleteVertexArrays(1, &buffer_id);
            buffer_id = 0;
        }
    }

    void vertex_format_buffer::bind_state() const
    {
        ZoneScoped;
        TracyGpuZone("bind vertex format buffer");
        glBindVertexArray(buffer_id);
    }

    void vertex_format_buffer::attach_vertex_buffer(const GLuint binding_slot, const GLuint vertex_buffer_id, const GLintptr vertex_buffer_offset, const GLsizei vertex_buffer_stride) const
    {
        ZoneScoped;
        TracyGpuZone("attach buffer to vertex format buffer");
        glVertexArrayVertexBuffer(buffer_id, binding_slot, vertex_buffer_id, vertex_buffer_offset, vertex_buffer_stride);
    }

    void vertex_format_buffer::attach_index_buffer(const GLuint index_buffer_id) const
    {
        ZoneScoped;
        TracyGpuZone("attach buffer to vertex format buffer");
        glVertexArrayElementBuffer(buffer_id, index_buffer_id);
    }

    void vertex_format_buffer::unbind_state()
    {
        ZoneScoped;
        TracyGpuZone("unbind vertex format buffer");
        glBindVertexArray(0);
    }

    GLuint vertex_format_buffer::get_id() const
    {
        return buffer_id;
    }
}
