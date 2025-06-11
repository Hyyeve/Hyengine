#include "graphics.hpp"
#include <stack>
#include <string>

#include "../common/colors.hpp"
#include "../core/logger.hpp"
#include "../library/gl.hpp"

namespace hyengine::graphics {

    using namespace hyengine;
    using namespace hyengine::common;

    using namespace glm;
    #pragma region Init

    bool try_load_glad()
    {
        return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0;
    }

    bool try_init_glfw()
    {
        return glfwInit() == GLFW_TRUE;
    }

    #pragma endregion

    #pragma region Debug

    static void handle_gl_error(GLenum source, GLenum type, GLuint  /*id*/, GLenum severity, GLsizei  /*length*/, const GLchar* message, const void*  /*userParam*/) {
        std::string source_str;
        std::string type_str;
        bool should_display = true;

        switch(source) {
            case GL_DEBUG_SOURCE_API:
                source_str = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                source_str = "Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                source_str = "Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                source_str = "Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                source_str = "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                source_str = "Other";
                break;
            default:
                source_str = "Unknown";
                break;
        }
        switch(type) {
            case GL_DEBUG_TYPE_ERROR:
                type_str = "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                type_str = "Deprecated Behavior";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                type_str = "Undefined Behavior";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                type_str = "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                type_str = "Performance";
                break;
            case GL_DEBUG_TYPE_MARKER:
                type_str = "Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                type_str = "Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                type_str = "Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER:
                should_display = false;
                type_str = "Other";
                break;
            default:
                type_str = "Unknown";
                break;
        }

        if (!should_display)
        {
            return;
        }

        std::string id_str = "GL " + source_str + " > " + type_str;
        std::string msg_str = std::string(message);

        switch(severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                logger::message_error( msg_str, id_str);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                logger::message_warn(msg_str, id_str);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                logger::message_info(msg_str, id_str);
                break;
            default:
                logger::message_debug(msg_str, id_str);
                break;
        }
    }

    void enable_gl_debug() {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(handle_gl_error, nullptr);
    }

    #pragma endregion

    #pragma region Scissor

    void set_scissor(const rectangle& rect) {
        set_scissor(rect.position, rect.size);
    }

    void set_scissor(const int x, const int y, const int width, const int height) {
        glScissor(x, y, width, height);
    }

    void set_scissor(ivec2 position, ivec2 size) {
        set_scissor(position.x, position.y, size.x, size.y);
    }


    void enable_scissor_test() {
        glEnable(GL_SCISSOR_TEST);
    }

    void disable_scissor_test() {
        glDisable(GL_SCISSOR_TEST);
    }

    #pragma endregion

    #pragma region Viewport

    static viewport current_viewport{};

    void set_viewport(const viewport& viewport) {
        current_viewport = viewport;
        glViewport(viewport.x_offset, viewport.y_offset, viewport.width, viewport.height);
    }

    const viewport& get_viewport() {
        return current_viewport;
    }

    viewport create_letterbox_viewport(const viewport& to_fit, const int target_width, const int target_height, const bool fill) {
        const float aspectA = to_fit.height * target_width;
        const float aspectB = target_height * to_fit.width;

        viewport result{};

        if (aspectA > aspectB != fill) {
            result.width = to_fit.width;
            result.height = aspectB / target_width;
        } else {
            result.width = aspectA / target_height;
            result.height = to_fit.height;
        }

        result.x_offset = (to_fit.width - result.width) / 2;
        result.y_offset = (to_fit.height - result.height) / 2;

        return result;
    }

    void set_gl_flag_enabled(const GLenum setting, const bool enable)
    {
        if (enable) glEnable(setting);
        else glDisable(setting);
    }

    void set_blending_config(const blending_config& config, const unsigned int buffer_slot)
    {
        glBlendColor(config.constant_blend_color.r, config.constant_blend_color.g, config.constant_blend_color.b, config.constant_blend_color.a);
        glBlendEquationSeparatei(buffer_slot, config.rgb_equation, config.alpha_equation);
        glBlendFuncSeparatei(buffer_slot, config.source_blend_rgb, config.dest_blend_rgb, config.source_blend_alpha, config.dest_blend_alpha);
    }

    void set_blending_enabled(const bool enable)
    {
        set_gl_flag_enabled(GL_BLEND, enable);
    }

    void set_stencil_config(const stencil_config& config, const GLenum facing)
    {
        glStencilFuncSeparate(facing, config.test_func, config.reference, config.test_mask);
        glStencilMaskSeparate(facing, config.write_mask);
        glStencilOpSeparate(facing, config.stencil_fail_op, config.depth_fail_op, config.pixel_pass_op);
    }

    void set_stencil_enabled(const bool enable)
    {
        set_gl_flag_enabled(GL_STENCIL_TEST, enable);
    }

    void set_cull_face_enabled(const bool enable)
    {
        set_gl_flag_enabled(GL_CULL_FACE, enable);
    }

    void set_cull_face_mode(const GLenum mode)
    {
        glCullFace(mode);
    }

    void set_depth_test_enabled(const bool enable)
    {
        set_gl_flag_enabled(GL_DEPTH_TEST, enable);
    }

    void set_cubemap_seamless_sampling(const bool enable)
    {
        set_gl_flag_enabled(GL_TEXTURE_CUBE_MAP_SEAMLESS, enable);
    }

    void set_clear_color(const glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void set_clear_depth(const float depth)
    {
        glClearDepthf(depth);
    }

    void set_clear_stencil(const int stencil)
    {
        glClearStencil(stencil);
    }

    void clear_buffers(const GLbitfield mask)
    {
        glClear(mask);
    }

    unsigned int get_max_texture_units()
    {
        GLint result = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &result);
        return static_cast<unsigned int>(result);
    }

#pragma endregion

}
