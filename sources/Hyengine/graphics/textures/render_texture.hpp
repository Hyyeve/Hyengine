#pragma once

#include "../../library/gl.hpp"
#include "../../library/glm.hpp"
#include "Hyengine/common/sized_numerics.hpp"


namespace hyengine
{
    class render_texture
    {
    public:
        render_texture(const render_texture& other) = delete; //COPY CONSTRUCTOR
        render_texture(render_texture&& other) = delete; //MOVE CONSTRUCTOR
        render_texture& operator=(const render_texture& other) = delete; //COPY ASSIGNMENT
        render_texture& operator=(render_texture&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit render_texture() = default;
        ~render_texture();

        void allocate(const GLenum format, const glm::uvec2 size, const i32 multisample_count);
        void free();

        void copy_data(const render_texture& source) const;
        void copy_data_partial(const render_texture& source, glm::uvec2 from_pos, glm::uvec2 to_pos, glm::uvec2 size) const;

        [[nodiscard]] GLuint get_id() const;
        [[nodiscard]] glm::uvec2 get_size() const;
        [[nodiscard]] GLenum get_format() const;
        [[nodiscard]] i32 get_multisample_count() const;

    private:
        const std::string logger_tag = "GPU Render Texture";

        GLuint gl_id = 0;
        glm::ivec2 internal_size = {0, 0};
        i32 internal_samples = 0;
        GLenum internal_format = 0;
    };
}
