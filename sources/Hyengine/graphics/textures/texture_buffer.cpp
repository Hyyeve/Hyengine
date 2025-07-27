#include "texture_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"
#include "../../library/gl.hpp"

namespace hyengine
{
    texture_buffer::~texture_buffer()
    {
        if (buffer_id > 0) free();
    }

    void texture_buffer::allocate(const GLenum texture_type, const glm::uvec3 size, const GLsizei mipmap_count, const GLenum format, const i32 multisample_count)
    {
        ZoneScoped;
        if (buffer_id > 0)
        {
            log_warn(logger_tag, "Attempted to initialize already initialized texture buffer!", " (buffer ", buffer_id, ")");
            return;
        }

        internal_size = size;
        internal_format = format;
        internal_texture_type = texture_type;
        internal_mipmaps = mipmap_count;
        internal_multisample = multisample_count;

        std::string type_info;

        glCreateTextures(internal_texture_type, 1, &buffer_id);
        switch (internal_texture_type)
        {
            case GL_TEXTURE_1D:
            case GL_PROXY_TEXTURE_1D:
            {
                glTextureStorage1D(buffer_id, internal_mipmaps, format, internal_size.x);
                type_info = stringify("1D, ", internal_size.x, " pixels, ", stringify_count(internal_mipmaps, "Mipmap level"));
                break;
            }

            case GL_TEXTURE_3D:
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_PROXY_TEXTURE_3D:
            case GL_PROXY_TEXTURE_2D_ARRAY:
            case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
            {
                glTextureStorage3D(buffer_id, internal_mipmaps, format, internal_size.x, internal_size.y, internal_size.z);
                type_info = stringify("3D / 2D Array, ", internal_size.x, "x", internal_size.y, "x", internal_size.z, ". ", stringify_count(internal_mipmaps, "Mipmap level"));
                break;
            }

            case GL_TEXTURE_2D_MULTISAMPLE:
            case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
            {
                //It's unclear whether the last parameter (fixed sample locations) is actually used by any implementations, so for now I'm not supporting customizing it.
                glTextureStorage2DMultisample(buffer_id, internal_multisample, format, internal_size.x, internal_size.y, GL_TRUE);
                type_info = stringify("2D multisample, ", internal_size.x, "x", internal_size.y, ". ", stringify_count(internal_multisample, "MSAA sample"));
                break;
            }

            case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
            {
                //It's unclear whether the last parameter (fixed sample locations) is actually used by any implementations, so for now I'm not supporting customizing it.
                glTextureStorage3DMultisample(buffer_id, internal_multisample, format, internal_size.x, internal_size.y, internal_size.z, GL_TRUE);
                type_info = stringify("2D multisample array, ", internal_size.x, "x", internal_size.y, "x", internal_size.z, ". ", stringify_count(internal_multisample, "MSAA sample"));
                break;
            }

            default:
            {
                glTextureStorage2D(buffer_id, internal_mipmaps, format, internal_size.x, internal_size.y);
                type_info = stringify("Default 2D, ", internal_size.x, "x", internal_size.y, ". ", stringify_count(internal_mipmaps, "Mipmap level"));
                break;
            }
        }

        log_info(logger_tag, "Allocated texture buffer ", buffer_id, ": ", type_info);
    }

    void texture_buffer::free()
    {
        ZoneScoped;
        glDeleteTextures(1, &buffer_id);
        log_info(logger_tag, "Freed texture buffer ", buffer_id, ".");
        buffer_id = 0;
    }

    void texture_buffer::copy_data(const texture_buffer& source) const
    {
        copy_data_mipmap(source, 0, 0);
    }

    void texture_buffer::copy_data_partial(const texture_buffer& source, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const
    {
        copy_data_mipmap_partial(source, 0, 0, from_pos, to_pos, size);
    }

    void texture_buffer::copy_data_mipmap(const texture_buffer& source, const i32 from_mipmap, const i32 to_mipmap) const
    {
        copy_data_mipmap_partial(source, from_mipmap, to_mipmap, {0, 0, 0}, {0, 0, 0}, internal_size);
    }

    void texture_buffer::copy_data_mipmap_partial(const texture_buffer& source, const i32 from_mipmap, const i32 to_mipmap, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const
    {
        ZoneScoped;
        glCopyImageSubData(source.get_id(), source.get_type(), from_mipmap, from_pos.x, from_pos.y, from_pos.z, buffer_id, internal_texture_type, to_mipmap, to_pos.x, to_pos.y, to_pos.z, size.x, size.y, size.z);
    }

    void texture_buffer::generate_mipmaps() const
    {
        ZoneScoped;
        glGenerateTextureMipmap(buffer_id);
    }


    void texture_buffer::upload_data_1d_partial(const GLint mip_level, const GLint offset, const GLint width, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        glTextureSubImage1D(buffer_id, mip_level, offset, width, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_1d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        upload_data_1d_partial(mip_level, 0, internal_size.x, data_format, data_type, data_pointer);
    }


    void texture_buffer::upload_data_2d_partial(const GLint mip_level, const glm::uvec2 offset, const glm::uvec2 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        glTextureSubImage2D(buffer_id, mip_level, offset.x, offset.y, size.x, size.y, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_2d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        upload_data_2d_partial(mip_level, {0, 0}, {internal_size.x, internal_size.y}, data_format, data_type, data_pointer);
    }


    void texture_buffer::upload_data_3d_partial(const GLint mip_level, const glm::uvec3 offset, const glm::uvec3 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        glTextureSubImage3D(buffer_id, mip_level, offset.x, offset.y, offset.z, size.x, size.y, size.z, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_3d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        upload_data_3d_partial(mip_level, {0, 0, 0}, internal_size, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_1d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        upload_data_2d_partial(mip_level, {0, layer}, {internal_size.x, 1}, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_2d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        upload_data_3d_partial(mip_level, {0, 0, layer}, {internal_size.x, internal_size.y, 1}, data_format, data_type, data_pointer);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id)
    {
        allocate_as_view(target, format, source_id, 0, 1, 0, 1);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id, const GLuint layer)
    {
        allocate_as_view(target, format, source_id, 0, 1, layer, 1);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id, const GLuint first_mip, const GLuint mipmap_count, const GLuint first_layer, const GLuint layer_count)
    {
        ZoneScoped;
        if (buffer_id != 0)
        {
            log_warn(logger_tag, "Attempted to initialize already initialized texture buffer!", " (buffer ", buffer_id, ")");
            return;
        }

        internal_format = format;
        internal_texture_type = target;

        glGenTextures(1, &buffer_id);
        glTextureView(buffer_id, internal_texture_type, source_id, internal_format, first_mip, mipmap_count, first_layer, layer_count);
        log_debug(logger_tag, "Allocated texture view (", buffer_id, ") of texture ", source_id);
    }


    void texture_buffer::set_depth_stencil_mode(const GLenum mode) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_DEPTH_STENCIL_TEXTURE_MODE, mode);
    }


    void texture_buffer::set_mipmap_min_level(const i32 level) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_BASE_LEVEL, level);
    }


    void texture_buffer::set_mipmap_max_level(const i32 level) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MAX_LEVEL, level);
    }


    void texture_buffer::set_border_color(const glm::vec4 col) const
    {
        ZoneScoped;
        glTextureParameterfv(buffer_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }


    void texture_buffer::set_compare_func(const GLenum func) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_COMPARE_FUNC, func);
    }


    void texture_buffer::set_compare_mode(const GLenum mode) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_COMPARE_MODE, mode);
    }

    void texture_buffer::set_anisotropy_level(const f32 anisotropy) const
    {
        ZoneScoped;
        if (GLAD_GL_VERSION_4_6 || GLAD_GL_ARB_texture_filter_anisotropic || GLAD_GL_EXT_texture_filter_anisotropic)
        {
            glTextureParameterf(buffer_id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
        }
    }

    void texture_buffer::set_downscale_filter(const GLenum filter) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MIN_FILTER, filter);
    }


    void texture_buffer::set_upscale_filter(const GLenum filter) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MAG_FILTER, filter);
    }


    void texture_buffer::set_lod_bias(const f32 bias) const
    {
        ZoneScoped;
        glTextureParameterf(buffer_id, GL_TEXTURE_LOD_BIAS, bias);
    }


    void texture_buffer::set_min_lod(const i32 lod) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MIN_LOD, lod);
    }


    void texture_buffer::set_max_lod(const i32 lod) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MAX_LOD, lod);
    }


    void texture_buffer::set_swizzle_red(const GLenum swizzle) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_SWIZZLE_R, swizzle);
    }


    void texture_buffer::set_swizzle_green(const GLenum swizzle) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_SWIZZLE_G, swizzle);
    }


    void texture_buffer::set_swizzle_blue(const GLenum swizzle) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_SWIZZLE_B, swizzle);
    }


    void texture_buffer::set_swizzle_alpha(const GLenum swizzle) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_SWIZZLE_A, swizzle);
    }


    void texture_buffer::set_texture_wrap_x(const GLenum wrap) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_WRAP_S, wrap);
    }


    void texture_buffer::set_texture_wrap_y(const GLenum wrap) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_WRAP_T, wrap);
    }


    void texture_buffer::set_texture_wrap_z(const GLenum wrap) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_WRAP_R, wrap);
    }


    void texture_buffer::bind(const i32 slot) const
    {
        ZoneScoped;
        glBindTextureUnit(slot, buffer_id);
    }


    void texture_buffer::bind_image(const i32 slot, const i32 level, const GLenum access) const
    {
        ZoneScoped;
        glBindImageTexture(slot, buffer_id, level, GL_FALSE, 0, access, internal_format);
    }


    void texture_buffer::bind_image_layered(const i32 slot, const i32 level, const i32 layer, const GLenum access) const
    {
        ZoneScoped;
        glBindImageTexture(slot, buffer_id, level, GL_TRUE, layer, access, internal_format);
    }

    GLuint texture_buffer::get_id() const
    {
        return buffer_id;
    }

    glm::uvec3 texture_buffer::get_size() const
    {
        return internal_size;
    }

    GLsizei texture_buffer::get_mipmap_count() const
    {
        return internal_mipmaps;
    }

    GLenum texture_buffer::get_type() const
    {
        return internal_texture_type;
    }

    GLenum texture_buffer::get_format() const
    {
        return internal_format;
    }

    i32 texture_buffer::get_multisample_count() const
    {
        return internal_multisample;
    }
}
