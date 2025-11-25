#pragma once

#include "../../common/sized_numerics.hpp"
#include "../../library/gl.hpp"
#include "../../library/glm.hpp"


namespace hyengine
{
    class render_texture
    {
    public:
        render_texture(const render_texture& other) = delete;                //COPY CONSTRUCTOR
        render_texture(render_texture&& other) = delete;                     //MOVE CONSTRUCTOR
        render_texture& operator=(const render_texture& other) = delete;     //COPY ASSIGNMENT
        render_texture& operator=(render_texture&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit render_texture() = default;
        ~render_texture();

        void allocate(const GLenum format, const glm::uvec2 size, const i32 multisample_count);
        void free();

        void copy_data_from(const render_texture& source) const;
        void copy_data_from(const render_texture& source, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const;

        void copy_data_from(const GLuint source) const;
        void copy_data_from(const GLuint source, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const;

        void copy_data_to(const GLuint dest) const;
        void copy_data_to(const GLuint dest, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const;

        static void copy_render_texture_data(const GLuint source, const GLuint dest, const glm::uvec2 source_pos, const glm::uvec2 dest_pos, const glm::uvec2 size);

        [[nodiscard]] GLuint get_id() const;
        [[nodiscard]] glm::uvec2 get_size() const;
        [[nodiscard]] GLenum get_format() const;
        [[nodiscard]] i32 get_multisample_count() const;
        [[nodiscard]] bool is_signed_integer_format() const;
        [[nodiscard]] bool is_unsigned_integer_format() const;
        [[nodiscard]] bool is_float_format() const;

    private:
        const std::string logger_tag = "GPU Render Texture";

        GLuint gl_id = 0;
        glm::uvec2 internal_size = {0, 0};
        i32 internal_samples = 0;
        GLenum internal_format = 0;
    };
}
