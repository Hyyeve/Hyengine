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

    std::stack<rectangle> scissor_stack;

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

    void push_scissor(const rectangle& rect, const ScissorMode mode, const bool replace) {
        rectangle scissor = rect;
        const bool empty = scissor_stack.empty();
        if (!empty && mode != ScissorMode::OVERWRITE) {
            const rectangle& prev = scissor_stack.top();
            switch (mode) {
                case ScissorMode::UNION:
                    scissor = prev.union_with(rect);
                    break;
                default:
                    scissor = prev.intersection_with(rect);
                    break;
            }
        }
        if(replace && !empty) scissor_stack.pop();
        scissor_stack.push(scissor);
        set_scissor(scissor);
        enable_scissor_test();
    }

    void push_scissor(const rectangle& rect, const ScissorMode mode) {
        push_scissor(rect, mode, false);
    }

    void replace_scissor(const rectangle& rect, const ScissorMode mode) {
        push_scissor(rect, mode, true);
    }

    void pop_scissor() {
        scissor_stack.pop();
        if (scissor_stack.empty()) disable_scissor_test();
        else set_scissor(scissor_stack.top());
    }

    const rectangle& peek_scissor() {
        return scissor_stack.top();
    }

    #pragma endregion

    #pragma region Viewport

    static viewport current_viewport{};

    void apply_viewport(const viewport& viewport) {
        current_viewport = viewport;
        glViewport(viewport.x_offset, viewport.y_offset, viewport.width, viewport.height);
    }

    const viewport& get_viewport() {
        return current_viewport;
    }

    viewport letterbox_viewport(const viewport& to_fit, const int target_width, const int target_height, const bool fill) {
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

    #pragma endregion

}
