#include "texture_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"
#include "../../library/gl.hpp"

namespace hyengine::graphics
{
    using namespace hyengine;

    texture_buffer::~texture_buffer()
    {
        ZoneScoped;
        if (buffer_id > 0) free();
    }

    void texture_buffer::allocate(const GLenum texture_type, const glm::uvec3 size, const GLsizei mipmap_count, const GLenum format, const int multisample_count)
    {
        ZoneScoped;
        if (buffer_id > 0)
        {
            logger::message_warn(logger::format("Attempted to initialize already initialized texture buffer!", " (buffer ", buffer_id, ")"), logger_tag);
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
                type_info = logger::format("1D, ", internal_size.x, " pixels, ", logger::format_count(internal_mipmaps, "Mipmap level"));
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
                type_info = logger::format("3D / 2D Array, ", internal_size.x, "x", internal_size.y, "x", internal_size.z, ". ", logger::format_count(internal_mipmaps, "Mipmap level"));
                break;
            }

            case GL_TEXTURE_2D_MULTISAMPLE:
            case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
            {
                //It's unclear whether the last parameter (fixed sample locations) is actually used by any implementations, so for now I'm not supporting customizing it.
                glTextureStorage2DMultisample(buffer_id, internal_multisample, format, internal_size.x, internal_size.y, GL_TRUE);
                type_info = logger::format("2D multisample, ", internal_size.x, "x", internal_size.y, ". ", logger::format_count(internal_multisample, "MSAA sample"));
                break;
            }

            case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
            {
                //It's unclear whether the last parameter (fixed sample locations) is actually used by any implementations, so for now I'm not supporting customizing it.
                glTextureStorage3DMultisample(buffer_id, internal_multisample, format, internal_size.x, internal_size.y, internal_size.z, GL_TRUE);
                type_info = logger::format("2D multisample array, ", internal_size.x, "x", internal_size.y, "x", internal_size.z, ". ", logger::format_count(internal_multisample, "MSAA sample"));
                break;
            }

            default:
            {
                glTextureStorage2D(buffer_id, internal_mipmaps, format, internal_size.x, internal_size.y);
                type_info = logger::format("Default 2D, ", internal_size.x, "x", internal_size.y, ". ", logger::format_count(internal_mipmaps, "Mipmap level"));
                break;
            }
        }

        logger::message_info(logger::format("Allocated texture buffer ", buffer_id, ": ", type_info), logger_tag);
    }

    void texture_buffer::free()
    {
        ZoneScoped;
        glDeleteTextures(1, &buffer_id);
        logger::message_info(logger::format("Freed texture buffer ", buffer_id, "."), logger_tag);
        buffer_id = 0;
    }

    void texture_buffer::copy_data(const texture_buffer& source) const
    {
        ZoneScoped;
        copy_data_mipmap(source, 0, 0);
    }

    void texture_buffer::copy_data_partial(const texture_buffer& source, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const
    {
        ZoneScoped;
        copy_data_mipmap_partial(source, 0, 0, from_pos, to_pos, size);
    }

    void texture_buffer::copy_data_mipmap(const texture_buffer& source, const int from_mipmap, const int to_mipmap) const
    {
        ZoneScoped;
        copy_data_mipmap_partial(source, from_mipmap, to_mipmap, {0, 0, 0}, {0, 0, 0}, internal_size);
    }

    void texture_buffer::copy_data_mipmap_partial(const texture_buffer& source, const int from_mipmap, const int to_mipmap, glm::uvec3 from_pos, glm::uvec3 to_pos, glm::uvec3 size) const
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
        ZoneScoped;
        upload_data_1d_partial(mip_level, 0, internal_size.x, data_format, data_type, data_pointer);
    }


    void texture_buffer::upload_data_2d_partial(const GLint mip_level, const glm::uvec2 offset, const glm::uvec2 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        glTextureSubImage2D(buffer_id, mip_level, offset.x, offset.y, size.x, size.y, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_2d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        upload_data_2d_partial(mip_level, {0, 0}, {internal_size.x, internal_size.y}, data_format, data_type, data_pointer);
    }


    void texture_buffer::upload_data_3d_partial(const GLint mip_level, const glm::uvec3 offset, const glm::uvec3 size, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        glTextureSubImage3D(buffer_id, mip_level, offset.x, offset.y, offset.z, size.x, size.y, size.z, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_3d(const GLint mip_level, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        upload_data_3d_partial(mip_level, {0, 0, 0}, internal_size, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_1d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        upload_data_2d_partial(mip_level, {0, layer}, {internal_size.x, 1}, data_format, data_type, data_pointer);
    }

    void texture_buffer::upload_data_2d_array_layer(const GLint mip_level, const GLint layer, const GLenum data_format, const GLenum data_type, const GLvoid* data_pointer) const
    {
        ZoneScoped;
        upload_data_3d_partial(mip_level, {0, 0, layer}, {internal_size.x, internal_size.y, 1}, data_format, data_type, data_pointer);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id)
    {
        ZoneScoped;
        allocate_as_view(target, format, source_id, 0, 1, 0, 1);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id, const GLuint layer)
    {
        ZoneScoped;
        allocate_as_view(target, format, source_id, 0, 1, layer, 1);
    }

    void texture_buffer::allocate_as_view(const GLenum target, const GLenum format, const GLuint source_id, const GLuint first_mip, const GLuint mipmap_count, const GLuint first_layer, const GLuint layer_count)
    {
        ZoneScoped;
        if (buffer_id != 0)
        {
            logger::message_warn(logger::format("Attempted to initialize already initialized texture buffer!", " (buffer ", buffer_id, ")"), logger_tag);
            return;
        }

        internal_format = format;
        internal_texture_type = target;

        glGenTextures(1, &buffer_id);
        glTextureView(buffer_id, internal_texture_type, source_id, internal_format, first_mip, mipmap_count, first_layer, layer_count);
        logger::message_debug(logger::format("Allocated texture view (", buffer_id, ") of texture ", source_id), logger_tag);
    }


    void texture_buffer::set_depth_stencil_mode(const GLenum mode) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_DEPTH_STENCIL_TEXTURE_MODE, mode);
    }


    void texture_buffer::set_mipmap_min_level(const int level) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_BASE_LEVEL, level);
    }


    void texture_buffer::set_mipmap_max_level(const int level) const
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

    void texture_buffer::set_anisotropy_level(const float anisotropy) const
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


    void texture_buffer::set_lod_bias(const float bias) const
    {
        ZoneScoped;
        glTextureParameterf(buffer_id, GL_TEXTURE_LOD_BIAS, bias);
    }


    void texture_buffer::set_min_lod(const int lod) const
    {
        ZoneScoped;
        glTextureParameteri(buffer_id, GL_TEXTURE_MIN_LOD, lod);
    }


    void texture_buffer::set_max_lod(const int lod) const
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


    void texture_buffer::bind(const int slot) const
    {
        ZoneScoped;
        glBindTextureUnit(slot, buffer_id);
    }


    void texture_buffer::bind_image(const int slot, const int level, const GLenum access) const
    {
        ZoneScoped;
        glBindImageTexture(slot, buffer_id, level, GL_FALSE, 0, access, internal_format);
    }


    void texture_buffer::bind_image_layered(const int slot, const int level, const int layer, const GLenum access) const
    {
        ZoneScoped;
        glBindImageTexture(slot, buffer_id, level, GL_TRUE, layer, access, internal_format);
    }

    GLuint texture_buffer::get_id() const
    {
        ZoneScoped;
        return buffer_id;
    }

    glm::uvec3 texture_buffer::get_size() const
    {
        ZoneScoped;
        return internal_size;
    }

    GLsizei texture_buffer::get_mipmap_count() const
    {
        ZoneScoped;
        return internal_mipmaps;
    }

    GLenum texture_buffer::get_type() const
    {
        ZoneScoped;
        return internal_texture_type;
    }

    GLenum texture_buffer::get_format() const
    {
        ZoneScoped;
        return internal_format;
    }

    int texture_buffer::get_multisample_count() const
    {
        ZoneScoped;
        return internal_multisample;
    }
}
