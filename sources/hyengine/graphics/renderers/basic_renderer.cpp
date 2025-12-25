#include "basic_renderer.hpp"

#include "../../common/colors.hpp"
#include "../../core/logger.hpp"
#include "../shaders/shader.hpp"
#include "../shaders/shader_batching.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    basic_renderer::basic_renderer() : draw_count(0), write_index(0) {}

    basic_renderer::~basic_renderer()
    {
        free();
    }

    bool basic_renderer::allocate(const u32 memory_budget_mb)
    {
        ZoneScoped;
        TracyGpuZone("allocate basic renderer");
        if (is_allocated)
        {
            log_warn(logger_tags::GRAPHICS, "Attempted to allocate already allocated basic renderer!");
            return true;
        }

        basic_shader = create_shader_instance("hyengine.shader.basic_pos_col", false);
        texture_shader = create_shader_instance("hyengine.shader.basic_pos_col_tex", true);

        const bool shader_allocated = basic_shader->allocate();
        const bool tex_shader_allocated = texture_shader->allocate();

        const u32 max_vertices = ((1024 * 1024) / sizeof(basic_vertex)) * memory_budget_mb;

        const bool vertex_buffer_allocated = vertex_buffer.allocate_for_cpu_writes(GL_ARRAY_BUFFER, max_vertices);
        const bool vertex_format_buffer_allocated = vertex_format_buffer.allocate();

        if (!shader_allocated || !tex_shader_allocated || !vertex_buffer_allocated || !vertex_format_buffer_allocated)
        {
            log_error(logger_tags::GRAPHICS, "Failed to allocate basic renderer!");
            return false;
        }

        vertex_format_buffer.attach_vertex_format(basic_vertex_format, 0);
        vertex_format_buffer.attach_vertex_buffer(0, vertex_buffer.get_buffer_id(), 0, vertex_buffer.get_element_size());

        is_allocated = true;

        log_debug(logger_tags::GRAPHICS, "Allocated debug renderer with ", stringify_bytes(max_vertices * sizeof(basic_vertex)), " of vertex memory.");
        return true;
    }

    void basic_renderer::free()
    {
        ZoneScoped;
        TracyGpuZone("free basic renderer");
        if (!is_allocated) return;

        basic_shader.reset();
        texture_shader.reset();

        vertex_format_buffer.free();
        vertex_buffer.free();
        draw_count = 0;
        write_index = 0;
        is_allocated = false;

        log_debug(logger_tags::GRAPHICS, "Freed debug renderer");
    }

    void basic_renderer::vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 uv)
    {
        if (!is_allocated)
        {
            log_warn(logger_tags::GRAPHICS, "Can't add triangle; renderer not allocated!");
            return;
        }

        vertex_buffer.get_mapped_slice_pointer()[write_index] = {pos, uv, color_to_bits(color)};
        write_index++;
    }

    void basic_renderer::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color)
    {
        vertex(a, color);
        vertex(c, color);
        vertex(b, color);
    }

    void basic_renderer::quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color)
    {
        vertex(a, color, glm::vec2(0.0f, 0.0f));
        vertex(b, color, glm::vec2(1.0f, 0.0f));
        vertex(c, color, glm::vec2(0.0f, 1.0f));

        vertex(d, color, glm::vec2(1.0f, 1.0f));
        vertex(c, color, glm::vec2(0.0f, 1.0f));
        vertex(b, color, glm::vec2(1.0f, 0.0f));
    }

    void basic_renderer::rect(glm::vec2 a, glm::vec2 b, glm::vec4 color)
    {
        quad({a.x, a.y, 0}, {b.x, a.y, 0}, {a.x, b.y, 0}, {b.x, b.y, 0}, color);
    }

    void basic_renderer::line(glm::vec3 start, glm::vec3 end, glm::vec4 color, f32 thickness)
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

    void basic_renderer::texture(const bool enable, const u32 texture_slot)
    {
        ZoneScoped;
        use_texture = enable;
        texture_shader->set_sampler_slot("u_texture", texture_slot);
    }

    void basic_renderer::finish()
    {
        draw_count = write_index;
        write_index = 0;
    }

    void basic_renderer::block_ready()
    {
        vertex_buffer.block_ready();
    }

    void basic_renderer::update_shader_uniforms(const f32 interpolation_delta, const camera& cam) const
    {
        ZoneScoped;
        shader* current_shader = use_texture ? texture_shader.get() : basic_shader.get();

        current_shader->set_uniform("u_projection_mat", cam.get_projection());
        current_shader->set_uniform("u_view_mat", cam.get_view());
        current_shader->set_uniform("u_camera_pos", cam.get_position(interpolation_delta));
    }

    void basic_renderer::reload_shaders() const
    {
        ZoneScoped;
        const bool basic_shader_reallocated = basic_shader->reallocate();
        const bool tex_shader_reallocated = texture_shader->reallocate();
        if (!basic_shader_reallocated || !tex_shader_reallocated)
        {
            log_error(logger_tags::GRAPHICS, "Reallocating basic renderer shaders failed!");
        }
    }

    void basic_renderer::bind() const
    {
        ZoneScoped;
        vertex_format_buffer.bind_state();
        if (use_texture) texture_shader->use();
        else basic_shader->use();
    }

    void basic_renderer::draw() const
    {
        ZoneScoped;
        TracyGpuZone("basic renderer draw");
        glDrawArrays(GL_TRIANGLES, vertex_buffer.get_slice_first_element(), draw_count);
    }
}
