#pragma once
#include "../camera.hpp"
#include "../buffers/vertex_format_buffer.hpp"
#include "../buffers/typed_data_buffer.hpp"
#include "Hyengine/graphics/shader.hpp"

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

        void allocate(const unsigned int memory_budget_mb);
        void free();

        void vertex(glm::vec3 pos, glm::vec4 color);
        void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
        void quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color);
        void rect(glm::vec2 a, glm::vec2 b, glm::vec4 color);
        void line(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness);

        void texture(bool enable, unsigned int texture_slot = 0);

        void finish();
        void block_ready();

        void update_shader_uniforms(const float interpolation_delta, const camera& cam);
        void reload_shaders();

        void bind();
        void draw() const;

    private:
        struct debug_vertex
        {
            glm::vec3 position;
            unsigned int color;
        };

        shader basic_shader = shader("shader:basic_pos_col");
        shader texture_shader = shader("shader:basic_pos_col_tex");

        const vertex_format<2> debug_vertex_format = vertex_format<2>({vertex_element_types::VEC3, vertex_element_types::UBYTE_NVEC4});
        const std::string logger_tag = "Basic Renderer";

        typed_data_buffer<debug_vertex> vertex_buffer;
        vertex_format_buffer vertex_format_buffer;

        bool is_allocated = false;
        unsigned int draw_count;
        unsigned int write_index;

        bool use_texture = false;
    };
}
