#include "basic_renderer.hpp"

#include "../../common/colors.hpp"
#include "../../core/logger.hpp"
#include "../shader.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    basic_renderer::basic_renderer(): draw_count(0), write_index(0)
    {
    }

    basic_renderer::~basic_renderer()
    {
        free();
    }

    void basic_renderer::allocate(const unsigned int memory_budget_mb)
    {
        if (is_allocated)
        {
            logger::warn(logger_tag, "Attempted to allocate already allocated debug renderer!");
            return;
        }

        const unsigned int max_vertices = ((1024 * 1024) / sizeof(debug_vertex)) * memory_budget_mb;

        vertex_buffer.allocate_for_continual_writes(GL_ARRAY_BUFFER, max_vertices);
        vertex_format_buffer.allocate();
        vertex_format_buffer.attach_vertex_format(debug_vertex_format, 0);
        vertex_format_buffer.attach_vertex_buffer(0, vertex_buffer.get_buffer_id(), 0, vertex_buffer.get_element_size());

        is_allocated = true;

        logger::info(logger_tag, "Allocated debug renderer with ", logger::format_bytes(max_vertices * sizeof(debug_vertex)), " of vertex memory.");
    }

    void basic_renderer::free()
    {
        if (!is_allocated) return;

        vertex_format_buffer.free();
        vertex_buffer.free();
        draw_count = 0;
        write_index = 0;
        is_allocated = false;

        logger::info(logger_tag, "Freed debug renderer");
    }

    void basic_renderer::vertex(glm::vec3 pos, glm::vec4 color)
    {
        if (!is_allocated)
        {
            logger::warn(logger_tag, "Can't add triangle; renderer not allocated!");
            return;
        }

        vertex_buffer.sync_blocking();
        vertex_buffer.get_mapped_slice_pointer()[write_index] = {pos, common::colors::float_to_bits(color)};
        write_index++;
    }

    void basic_renderer::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color)
    {
        vertex(a, color);
        vertex(b, color);
        vertex(c, color);
    }

    void basic_renderer::quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color)
    {
        triangle(a, b, c, color);
        triangle(d, c, b, color);
    }

    void basic_renderer::rect(glm::vec2 a, glm::vec2 b, glm::vec4 color)
    {
        quad({a.x, a.y, 0}, {b.x, a.y, 0}, {a.x, b.y, 0}, {b.x, b.y, 0}, color);
    }

    void basic_renderer::line(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness)
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

    void basic_renderer::texture(const bool enable, const unsigned int texture_slot)
    {
        use_texture = enable;
        texture_shader.set_sampler_slot("u_texture", texture_slot);
    }

    void basic_renderer::submit_geometry()
    {
        draw_count = write_index;
        vertex_buffer.flush_and_fence();
        write_index = 0;
    }

    void basic_renderer::prepare_buffers()
    {
        vertex_buffer.increment_slice();
    }

    void basic_renderer::update_shader_uniforms(const float interpolation_delta, const graphics::camera& cam)
    {
        shader& current_shader = use_texture ? texture_shader : basic_shader;

        current_shader.set_uniform("u_projection_mat", cam.get_projection());
        current_shader.set_uniform("u_view_mat", cam.get_view());
        current_shader.set_uniform("u_camera_pos", cam.get_position(interpolation_delta));
    }

    void basic_renderer::reload_shaders()
    {
        basic_shader.reload();
        texture_shader.reload();
    }

    void basic_renderer::bind()
    {
        vertex_format_buffer.bind_state();
        if (use_texture) texture_shader.use();
        else basic_shader.use();
    }

    void basic_renderer::draw() const
    {
        glDrawArrays(GL_TRIANGLES, vertex_buffer.get_slice_first_element(), draw_count);
    }
}
