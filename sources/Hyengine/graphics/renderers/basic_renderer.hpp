#pragma once
#include "../camera.hpp"
#include "../buffers/vertex_format_buffer.hpp"
#include "../buffers/typed_data_buffer.hpp"
#include "../shader.hpp"

namespace hyengine
{
    class basic_renderer
    {
    public:
        explicit basic_renderer();

        basic_renderer(const basic_renderer& other) = delete; //COPY CONSTRUCTOR
        basic_renderer(basic_renderer&& other) = delete; //MOVE CONSTRUCTOR
        basic_renderer& operator=(const basic_renderer& other) = delete; //COPY ASSIGNMENT
        basic_renderer& operator=(basic_renderer&& other) noexcept = delete; //MOVE ASSIGNMENT

        ~basic_renderer();

        void allocate(const u32 memory_budget_mb);
        void free();

        void vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 uv = glm::vec2(0.0, 0.0));
        void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
        void quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color);
        void rect(glm::vec2 a, glm::vec2 b, glm::vec4 color);
        void line(glm::vec3 start, glm::vec3 end, glm::vec4 color, f32 thickness);

        void texture(bool enable, u32 texture_slot = 0);

        void finish();
        void block_ready();

        void update_shader_uniforms(const f32 interpolation_delta, const camera& cam);
        void reload_shaders();

        void bind();
        void draw() const;

    private:
        struct basic_vertex
        {
            glm::vec3 position;
            glm::vec2 uv;
            u32 color;
        };

        std::shared_ptr<shader> basic_shader;
        std::shared_ptr<shader> texture_shader;

        const vertex_format<3> basic_vertex_format = vertex_format<3>({vertex_element_types::VEC3, vertex_element_types::VEC2, vertex_element_types::UBYTE_NVEC4});
        const std::string logger_tag = "Basic Renderer";

        typed_data_buffer<basic_vertex> vertex_buffer;
        vertex_format_buffer vertex_format_buffer;

        bool is_allocated = false;
        u32 draw_count;
        u32 write_index;

        bool use_texture = false;
    };
}
