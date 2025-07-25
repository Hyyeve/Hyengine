#pragma once
#include "../../library/gl.hpp"
#include "../../library/glm.hpp"

namespace hyengine
{
    class texture_buffer
    {
    public:
        texture_buffer(const texture_buffer& other) = delete; //COPY CONSTRUCTOR
        texture_buffer(texture_buffer&& other) = delete; //MOVE CONSTRUCTOR
        texture_buffer& operator=(const texture_buffer& other) = delete; //COPY ASSIGNMENT
        texture_buffer& operator=(texture_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit texture_buffer() = default;
        ~texture_buffer();

        void allocate(const GLenum texture_type, const glm::uvec3 size, const GLsizei mipmap_count, const GLenum format, const int multisample_count);
        void free();

        void allocate_as_view(const GLenum target, const GLenum format, GLuint source_id);
        void allocate_as_view(const GLenum target, const GLenum format, GLuint source_id, GLuint layer);
        void allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id, const GLuint first_mip, const GLuint mipmap_count, const GLuint first_layer, const GLuint layer_count);

        void upload_data_1d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;
        void upload_data_2d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;
        void upload_data_3d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;

        void upload_data_1d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;
        void upload_data_2d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;

        void upload_data_1d_partial(const GLint mip_level, const GLint offset, const GLint width, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;
        void upload_data_2d_partial(const GLint mip_level, const glm::uvec2 offset, const glm::uvec2 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;
        void upload_data_3d_partial(const GLint mip_level, const glm::uvec3 offset, const glm::uvec3 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;

        void copy_data(const texture_buffer& source) const;
        void copy_data_partial(const texture_buffer& source, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const;
        void copy_data_mipmap(const texture_buffer& source, const int from_mipmap, const int to_mipmap) const;
        void copy_data_mipmap_partial(const texture_buffer& source, const int from_mipmap, const int to_mipmap, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const;

        void generate_mipmaps() const;
        void set_depth_stencil_mode(const GLenum mode) const;

        void set_mipmap_min_level(const int level) const;
        void set_mipmap_max_level(const int level) const;

        void set_border_color(const glm::vec4 col) const;

        void set_compare_func(const GLenum func) const;
        void set_compare_mode(const GLenum mode) const;

        void set_anisotropy_level(const float anisotropy) const;

        void set_downscale_filter(const GLenum filter) const;
        void set_upscale_filter(const GLenum filter) const;

        void set_lod_bias(const float bias) const;
        void set_min_lod(const int lod) const;
        void set_max_lod(const int lod) const;

        void set_swizzle_red(const GLenum swizzle) const;
        void set_swizzle_green(const GLenum swizzle) const;
        void set_swizzle_blue(const GLenum swizzle) const;
        void set_swizzle_alpha(const GLenum swizzle) const;

        void set_texture_wrap_x(const GLenum wrap) const;
        void set_texture_wrap_y(const GLenum wrap) const;
        void set_texture_wrap_z(const GLenum wrap) const;

        void bind(const int slot) const;
        void bind_image(const int slot, const int level, GLenum access) const;
        void bind_image_layered(const int slot, const int level, const int layer, const GLenum access) const;

        [[nodiscard]] GLuint get_id() const;
        [[nodiscard]] glm::uvec3 get_size() const;
        [[nodiscard]] GLsizei get_mipmap_count() const;
        [[nodiscard]] GLenum get_type() const;
        [[nodiscard]] GLenum get_format() const;
        [[nodiscard]] int get_multisample_count() const;

    private:
        const std::string logger_tag = "GPU Texture";

        GLuint buffer_id = 0;
        GLenum internal_texture_type = 0;
        glm::ivec3 internal_size = glm::ivec3(0);
        GLenum internal_format = 0;
        GLsizei internal_mipmaps = 0;
        int internal_multisample = 0;
    };
}
