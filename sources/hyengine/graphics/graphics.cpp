#include "graphics.hpp"
#include <string>

#include "gl_enums.hpp"
#include "../common/colors.hpp"
#include "../core/logger.hpp"
#include "../library/gl.hpp"

namespace hyengine
{

    bool try_init_glfw()
    {
        ZoneScoped;
        return glfwInit() == GLFW_TRUE;
    }

    bool try_load_glad()
    {
        ZoneScoped;
        return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0;
    }

    static void handle_gl_error(GLenum source, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
    {
        std::string source_str;
        std::string type_str;

        switch (source)
        {
            case GL_DEBUG_SOURCE_API: source_str = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER: source_str = "Other";
                break;
            default: source_str = "Unknown";
                break;
        }
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: type_str = "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behavior";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "Undefined Behavior";
                break;
            case GL_DEBUG_TYPE_PORTABILITY: type_str = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE: type_str = "Performance";
                break;
            case GL_DEBUG_TYPE_MARKER: type_str = "Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP: type_str = "Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER: type_str = "Other";
                break;
            default: type_str = "Unknown";
                break;
        }

        std::string id_str = stringify("GL ", source_str, " > ", type_str);
        std::string msg_str = std::string(message);

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: log_error(logger_tags::tag { id_str, "" }, msg_str);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM: log_warn(logger_tags::tag { id_str, "" }, msg_str);
                break;
            case GL_DEBUG_SEVERITY_LOW: log_info(logger_tags::tag { id_str, "" }, msg_str);
                break;
            default: log_debug(logger_tags::tag { id_str, "" }, msg_str);
                break;
        }
    }

    void enable_gl_debug()
    {
        set_gl_flag_enabled(gl_flags::DEBUG_OUTPUT, true);
        glDebugMessageCallback(handle_gl_error, nullptr);
        log_debug(logger_tags::GRAPHICS, "Enabled GL debug messaging");
    }

    void enable_scissor_test()
    {
        ZoneScoped;
        set_gl_flag_enabled(gl_flags::SCISSOR_TEST, true);
    }

    void disable_scissor_test()
    {
        ZoneScoped;
        set_gl_flag_enabled(gl_flags::SCISSOR_TEST, false);
    }

    void set_scissor(const i32 x, const i32 y, const i32 width, const i32 height)
    {
        ZoneScoped;
        glScissor(x, y, width, height);
    }

    void set_scissor(glm::ivec2 position, glm::ivec2 size)
    {
        set_scissor(position.x, position.y, size.x, size.y);
    }

    void set_scissor(const aa_rectangle& rect)
    {
        set_scissor(rect.position, rect.extents);
    }

    static render_viewport current_viewport {};

    void set_viewport(const render_viewport& viewport)
    {
        ZoneScoped;
        current_viewport = viewport;
        glViewport(viewport.x_offset, viewport.y_offset, static_cast<i32>(viewport.width), static_cast<i32>(viewport.height));
    }

    const render_viewport& get_viewport()
    {
        return current_viewport;
    }

    render_viewport create_fitted_viewport(const render_viewport& to_fit, const i32 target_width, const i32 target_height, const bool fill)
    {
        const u32 aspect_a = to_fit.height * target_width;
        const u32 aspect_b = target_height * to_fit.width;

        render_viewport result {};

        if ((aspect_a > aspect_b) != fill)
        {
            result.width = to_fit.width;
            result.height = aspect_b / target_width;
        }
        else
        {
            result.width = aspect_a / target_height;
            result.height = to_fit.height;
        }

        result.x_offset = (static_cast<i32>(to_fit.width) - static_cast<i32>(result.width)) / 2;
        result.y_offset = (static_cast<i32>(to_fit.height) - static_cast<i32>(result.height)) / 2;

        return result;
    }

    static std::unordered_map<GLenum, bool> cached_gl_flags;

    void set_gl_flag_enabled(const GLenum setting, const bool enable)
    {
        ZoneScoped;
        if (cached_gl_flags.contains(setting))
        {
            const bool current = cached_gl_flags[setting];
            if (current == enable)
            {
                log_debug(logger_tags::GRAPHICS, "GL flag already set to target value");
                return;
            }
        }

        if (enable)
        {
            glEnable(setting);
        }
        else
        {
            glDisable(setting);
        }

        cached_gl_flags[setting] = enable;
    }

    bool get_gl_flag_enabled(const GLenum setting)
    {
        if (cached_gl_flags.contains(setting)) return cached_gl_flags[setting];
        const bool value = glIsEnabled(setting);
        cached_gl_flags[setting] = value;
        return value;
    }

    i32 get_gl_const_i32(const GLenum constant)
    {
        static std::unordered_map<GLenum, u32> cached_constants;
        if (cached_constants.contains(constant)) return cached_constants.at(constant);

        glm::ivec4 values;
        glGetIntegerv(constant, glm::value_ptr(values));
        cached_constants[constant] = values.x;
        return values.x;
    }

    void set_blending_config(const blending_config& config, const u32 buffer_slot)
    {
        ZoneScoped;
        glBlendColor(config.constant_blend_color.r, config.constant_blend_color.g, config.constant_blend_color.b, config.constant_blend_color.a);
        glBlendEquationSeparatei(buffer_slot, config.rgb_equation, config.alpha_equation);
        glBlendFuncSeparatei(buffer_slot, config.source_blend_rgb, config.dest_blend_rgb, config.source_blend_alpha, config.dest_blend_alpha);
    }

    void set_blending_enabled(const bool enable)
    {
        ZoneScoped;
        set_gl_flag_enabled(gl_flags::BLENDING, enable);
    }

    void set_stencil_config(const stencil_config& config, const GLenum facing)
    {
        ZoneScoped;
        glStencilFuncSeparate(facing, config.test_func, config.reference, config.test_mask);
        glStencilMaskSeparate(facing, config.write_mask);
        glStencilOpSeparate(facing, config.stencil_fail_op, config.depth_fail_op, config.pixel_pass_op);
    }

    void set_stencil_enabled(const bool enable)
    {
        set_gl_flag_enabled(gl_flags::STENCIL_TEST, enable);
    }

    void set_cull_face_enabled(const bool enable)
    {
        set_gl_flag_enabled(gl_flags::FACE_CULLING, enable);
    }

    void set_cull_face_mode(const GLenum mode)
    {
        ZoneScoped;
        glCullFace(mode);
    }

    void set_depth_test_enabled(const bool enable)
    {
        set_gl_flag_enabled(gl_flags::DEPTH_TEST, enable);
    }

    void set_cubemap_seamless_sampling(const bool enable)
    {
        set_gl_flag_enabled(gl_flags::SEAMLESS_CUBE_MAP, enable);
    }

    void set_clear_color(const glm::vec4 color)
    {
        ZoneScoped;
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void set_clear_depth(const f32 depth)
    {
        ZoneScoped;
        glClearDepthf(depth);
    }

    void set_clear_stencil(const i32 stencil)
    {
        ZoneScoped;
        glClearStencil(stencil);
    }

    void clear_buffers(const GLbitfield mask)
    {
        ZoneScoped;
        glClear(mask);
    }
}
