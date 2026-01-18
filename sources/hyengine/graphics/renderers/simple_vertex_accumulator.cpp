#include "simple_vertex_accumulator.hpp"

#include "../../common/colors.hpp"
#include "../../core/logger.hpp"
#include "../shaders/shader.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    simple_vertex_accumulator::simple_vertex_accumulator() : vertex_count(0), write_index(0) {}

    simple_vertex_accumulator::~simple_vertex_accumulator()
    {
        free();
    }

    bool simple_vertex_accumulator::allocate(const u32 memory_budget_mb)
    {
        ZoneScoped;
        TracyGpuZone("allocate basic renderer");
        if (is_allocated)
        {
            log_warn(logger_tags::GRAPHICS, "Attempted to allocate already allocated basic renderer!");
            return true;
        }

        const u32 max_vertices = ((1024 * 1024) / sizeof(simple_vertex)) * memory_budget_mb;

        const bool vertex_buffer_allocated = vertex_buffer.allocate_for_cpu_writes(max_vertices);
        const bool vertex_format_buffer_allocated = vertex_format_buffer.allocate();

        if (!vertex_buffer_allocated || !vertex_format_buffer_allocated)
        {
            log_error(logger_tags::GRAPHICS, "Failed to allocate basic renderer!");
            return false;
        }

        vertex_format_buffer.attach_vertex_format<3>(basic_vertex_format);
        vertex_format_buffer.attach_vertex_buffer(0, vertex_buffer.get_buffer_id(), 0, vertex_buffer.get_element_size());

        is_allocated = true;

        log_debug(logger_tags::GRAPHICS, "Allocated debug renderer with ", stringify_bytes(max_vertices * sizeof(simple_vertex)), " of vertex memory.");
        return true;
    }

    void simple_vertex_accumulator::free()
    {
        ZoneScoped;
        TracyGpuZone("free basic renderer");
        if (!is_allocated) return;

        vertex_format_buffer.free();
        vertex_buffer.free();
        vertex_count = 0;
        write_index = 0;
        is_allocated = false;

        log_debug(logger_tags::GRAPHICS, "Freed debug renderer");
    }

    void simple_vertex_accumulator::vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 uv)
    {
        if (!is_allocated)
        {
            log_warn(logger_tags::GRAPHICS, "Can't add triangle; renderer not allocated!");
            return;
        }

        vertex_buffer.get_mapped_slice_pointer()[write_index] = {pos, uv, color_to_bits(color)};
        write_index++;
    }

    void simple_vertex_accumulator::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color)
    {
        vertex(a, color);
        vertex(c, color);
        vertex(b, color);
    }

    void simple_vertex_accumulator::quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color)
    {
        vertex(a, color, glm::vec2(0.0f, 0.0f));
        vertex(b, color, glm::vec2(1.0f, 0.0f));
        vertex(c, color, glm::vec2(0.0f, 1.0f));

        vertex(d, color, glm::vec2(1.0f, 1.0f));
        vertex(c, color, glm::vec2(0.0f, 1.0f));
        vertex(b, color, glm::vec2(1.0f, 0.0f));
    }

    void simple_vertex_accumulator::rect(glm::vec2 a, glm::vec2 b, glm::vec4 color)
    {
        quad({a.x, a.y, 0}, {b.x, a.y, 0}, {a.x, b.y, 0}, {b.x, b.y, 0}, color);
    }

    void simple_vertex_accumulator::line(glm::vec3 start, glm::vec3 end, glm::vec4 color, f32 thickness)
    {
        const glm::vec3 axis = glm::normalize(end - start);
        const glm::vec3 tangent = glm::normalize(glm::cross(axis, glm::normalize(glm::vec3(1e-3, 1e-2, 1e-4))));
        const glm::vec3 bitangent = glm::normalize(glm::cross(axis, tangent));

        const glm::vec3 tangent_offset = tangent * thickness;
        const glm::vec3 bitangent_offset = bitangent * thickness;

        const glm::vec3 offset_0 = tangent_offset + bitangent_offset;
        const glm::vec3 offset_1 = tangent_offset - bitangent_offset;
        const glm::vec3 offset_2 = -tangent_offset + bitangent_offset;
        const glm::vec3 offset_3 = -tangent_offset - bitangent_offset;

        quad(start + offset_1, start + offset_0, end + offset_1, end + offset_0, color);
        quad(start + offset_2, start + offset_3, end + offset_2, end + offset_3, color);
        quad(start + offset_0, start + offset_2, end + offset_0, end + offset_2, color);
        quad(start + offset_3, start + offset_1, end + offset_3, end + offset_1, color);
        quad(start + offset_0, start + offset_1, start + offset_2, start + offset_3, color);
        quad(end + offset_2, end + offset_3, end + offset_0, end + offset_1, color);
    }

    void simple_vertex_accumulator::finish()
    {
        vertex_count = write_index;
        write_index = 0;
    }

    void simple_vertex_accumulator::next_slice()
    {
        vertex_buffer.next_slice();
    }

    void simple_vertex_accumulator::bind_state() const
    {
        ZoneScoped;
        vertex_format_buffer.bind_state();
    }

    u32 simple_vertex_accumulator::get_vertex_count() const
    {
        return vertex_count;
    }

    u32 simple_vertex_accumulator::get_vertex_start() const
    {
        return vertex_buffer.get_slice_first_element();
    }
}
