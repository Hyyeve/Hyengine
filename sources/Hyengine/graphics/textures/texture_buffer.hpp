#pragma once
#include "../../library/gl.hpp"
#include "../../library/glm.hpp"
#include "Hyengine/common/sized_numerics.hpp"

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

        void allocate(const GLenum texture_type, const glm::uvec3& size, const GLsizei mipmap_count, const GLenum format, const i32 multisample_count);
        void free();

        void clear(const u32 level, const GLenum data_format, const GLenum data_type, const void* data) const;

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
        void upload_data_3d_partial(const GLint mip_level, const glm::uvec3& offset, const glm::uvec3& size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const;

        void copy_data_from(const texture_buffer& source) const;
        void copy_data_from(const texture_buffer& source, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;
        void copy_data_from(const texture_buffer& source, const i32 from_mipmap, const i32 to_mipmap) const;
        void copy_data_from(const texture_buffer& source, const i32 from_mipmap, const i32 to_mipmap, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;

        void copy_data_from(const GLuint source, const GLenum source_texture_type) const;
        void copy_data_from(const GLuint source, const GLenum source_texture_type, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;
        void copy_data_from(const GLuint source, const GLenum source_texture_type, const i32 from_mipmap, const i32 to_mipmap, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;

        void copy_data_to(const GLuint dest, const GLenum dest_texture_type) const;
        void copy_data_to(const GLuint dest, const GLenum dest_texture_type, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;
        void copy_data_to(const GLuint dest, const GLenum dest_texture_type, const i32 from_mipmap, const i32 to_mipmap, const glm::uvec3& from_pos, const glm::uvec3& to_pos, const glm::uvec3& size) const;

        static void copy_texture_data(const GLuint source, const GLenum source_type, const GLuint dest, const GLenum dest_type, const i32 source_level, const i32 dest_level, const glm::uvec3& source_pos, const glm::uvec3& dest_pos, const glm::uvec3& size);

        void generate_mipmaps() const;
        void set_depth_stencil_mode(const GLenum mode) const;

        void set_mipmap_min_level(const i32 level) const;
        void set_mipmap_max_level(const i32 level) const;

        void set_border_color(const glm::vec4 col) const;

        void set_compare_func(const GLenum func) const;
        void set_compare_mode(const GLenum mode) const;

        void set_anisotropy_level(const f32 anisotropy) const;

        void set_downscale_filter(const GLenum filter) const;
        void set_upscale_filter(const GLenum filter) const;

        void set_lod_bias(const f32 bias) const;
        void set_min_lod(const i32 lod) const;
        void set_max_lod(const i32 lod) const;

        void set_swizzle_red(const GLenum swizzle) const;
        void set_swizzle_green(const GLenum swizzle) const;
        void set_swizzle_blue(const GLenum swizzle) const;
        void set_swizzle_alpha(const GLenum swizzle) const;

        void set_texture_wrap_x(const GLenum wrap) const;
        void set_texture_wrap_y(const GLenum wrap) const;
        void set_texture_wrap_z(const GLenum wrap) const;

        void bind(const i32 slot) const;
        void bind_image(const i32 slot, const i32 level, GLenum access) const;
        void bind_image_layered(const i32 slot, const i32 level, const i32 layer, const GLenum access) const;

        [[nodiscard]] GLuint get_id() const;
        [[nodiscard]] glm::uvec3 get_size() const;
        [[nodiscard]] GLsizei get_mipmap_count() const;
        [[nodiscard]] GLenum get_type() const;
        [[nodiscard]] GLenum get_format() const;
        [[nodiscard]] i32 get_multisample_count() const;
        [[nodiscard]] bool is_signed_integer_format() const;
        [[nodiscard]] bool is_unsigned_integer_format() const;
        [[nodiscard]] bool is_float_format() const;

    private:
        const std::string logger_tag = "GPU Texture";

        GLuint buffer_id = 0;
        GLenum internal_texture_type = 0;
        glm::ivec3 internal_size = glm::ivec3(0);
        GLenum internal_format = 0;
        GLsizei internal_mipmaps = 0;
        i32 internal_multisample = 0;
    };
}
