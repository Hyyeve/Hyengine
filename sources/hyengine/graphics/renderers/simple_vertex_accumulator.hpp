#pragma once
#include "../camera.hpp"
#include "../buffers/typed_data_buffer.hpp"
#include "../buffers/vertex_format_buffer.hpp"
#include "../shaders/shader.hpp"

namespace hyengine
{
    ///Simple vertex buffer builder for rendering geometry that's often modified \n
    ///Stores vertices in the format: @code
    ///0: vec3 position
    ///1: vec2 uv
    ///2: vec4 color (8-bit precision)
    class simple_vertex_accumulator
    {
    public:
        explicit simple_vertex_accumulator();

        simple_vertex_accumulator(const simple_vertex_accumulator& other) = delete;                //COPY CONSTRUCTOR
        simple_vertex_accumulator(simple_vertex_accumulator&& other) = delete;                     //MOVE CONSTRUCTOR
        simple_vertex_accumulator& operator=(const simple_vertex_accumulator& other) = delete;     //COPY ASSIGNMENT
        simple_vertex_accumulator& operator=(simple_vertex_accumulator&& other) noexcept = delete; //MOVE ASSIGNMENT

        ~simple_vertex_accumulator();

        [[nodiscard]] bool allocate(const u32 memory_budget_mb);
        void free();

        void vertex(glm::vec3 pos, glm::vec4 color, glm::vec2 uv = glm::vec2(0.0, 0.0));

        void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
        void quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color);
        void rect(glm::vec2 a, glm::vec2 b, glm::vec4 color);
        void line(glm::vec3 start, glm::vec3 end, glm::vec4 color, f32 thickness);

        void finish();
        void next_slice();

        void bind_state() const;

        [[nodiscard]] u32 get_vertex_count() const;
        [[nodiscard]] u32 get_vertex_start() const;

    private:
        struct simple_vertex
        {
            glm::vec3 position;
            glm::vec2 uv;
            u32 color;
        };

        static constexpr vertex_format<3> basic_vertex_format = vertex_format<3>({vertex_element_types::VEC3, vertex_element_types::VEC2, vertex_element_types::UBYTE_NVEC4});

        typed_data_buffer<simple_vertex> vertex_buffer;
        vertex_format_buffer vertex_format_buffer;

        u32 write_index;
        u32 vertex_count;
        bool is_allocated = false;
    };
}
