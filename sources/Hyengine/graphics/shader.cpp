#include "shader.hpp"

#include <array>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

#include "../common/file_io.hpp"

namespace hyengine::graphics {

    using namespace hyengine;
    using namespace glm;

    shader::shader(std::string asset_id, const bool preload) : asset_id(std::move(asset_id))
    {
        if(preload) load();
    }

    shader::shader(std::string asset_id) noexcept : asset_id(std::move(asset_id)) {}

    shader::~shader()
    {
        unload();
    }

    bool shader::valid() const
    {
        return program != 0;
    }

    bool shader::active() const
    {
        int current_program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        return current_program == program;
    }

    void shader::use()
    {
        if(!initialized) load();
        glUseProgram(program);
    }

    void shader::load()
    {
        if(initialized) return;
        program = load_program(asset_id);
        if(program != 0)
        {
            load_uniform_locations();
            logger::message_info(logger::format("Loaded shader ", asset_id, " and located ", uniform_locations.size(), " uniforms"), logger_tag);
        }
        else
        {
            logger::message_error(logger::format("Shader initialization for ", asset_id, " failed!"), logger_tag);
        }
        initialized = true;
    }

    void shader::unload() {
        if(!initialized) return;
        glDeleteProgram(program);
        program = 0;
        initialized = false;
        uniform_locations.clear();
    }

    void shader::reload()
    {
        const bool in_use = active();
        unload();
        load();
        if(in_use) use();
    }


    static GLuint compile_shader(const std::string& share, const std::string& source, const GLuint type)
    {
        const std::string combined = share + "\n" + source;
        const char* c_source = combined.c_str();

        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &c_source, nullptr);
        glCompileShader(shader);

        GLint success = GL_TRUE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE)
        {
            int log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            std::string log;
            log.resize(max(log_length, 0));

            glGetShaderInfoLog(shader, log_length, nullptr, log.data());

            log.insert(0, "\n ---------- Failed to compile shader: \n ");
            log.insert(log.length() - 1, " ---------------------------------------- ");

            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, log.length(), log.data());
            return 0;
        }

        return shader;
    }

    GLuint shader::link_program(const std::vector<GLuint>& shaders)
    {
        const GLuint program = glCreateProgram();
        for (const GLuint shader : shaders)
        {
            if(shader != 0) glAttachShader(program, shader);
        }

        glLinkProgram(program);

        GLint success = GL_TRUE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            int log_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

            std::string log;
            log.resize(max(log_length, 0));

            glGetProgramInfoLog(program, log_length, nullptr, log.data());

            log.insert(0, "\n ---------- Failed to link program: \n");
            log.insert(log.length() - 1, "\n ---------------------------------------- ");

            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, log.length(), log.data());
            return 0;
        }

        return program;
    }

    static bool line_contains(const std::string_view line, const std::string_view str)
    {
        return line.find(str) != std::string::npos;
    }

    GLuint shader::load_program(const std::string& asset_id) {

        const std::string raw_text = common::file_io::read_asset_text(asset_id);
        std::istringstream text(raw_text);

        std::string line;
        std::array<std::stringstream, 7> stage_sources {};

        int current_line_type = -1;
        int previous_line_type = current_line_type;
        while (getline(text, line))
        {
            if (line_contains(line,"#SHADER|VERTEX")) current_line_type = 0;
            else if (line_contains(line,"#SHADER|FRAGMENT")) current_line_type = 1;
            else if (line_contains(line,"#SHADER|COMPUTE")) current_line_type = 2;
            else if (line_contains(line,"#SHADER|GEOMETRY")) current_line_type = 3;
            else if (line_contains(line,"#SHADER|TESS_CONTROL")) current_line_type = 4;
            else if (line_contains(line,"#SHADER|TESS_EVAL")) current_line_type = 5;
            else if (line_contains(line,"#SHADER|SHARED")) current_line_type = 6;

            if(current_line_type == -1 || current_line_type != previous_line_type) {
                previous_line_type = current_line_type;
                continue; //Skip including the type define lines
            }

            stage_sources[current_line_type] << line << '\n';
        }

        const std::string share = stage_sources[6].str();

        std::vector<GLuint> shaders;
        if(!stage_sources[0].str().empty()) shaders.push_back(compile_shader(share, stage_sources[0].str(), GL_VERTEX_SHADER));
        if(!stage_sources[1].str().empty()) shaders.push_back(compile_shader(share, stage_sources[1].str(), GL_FRAGMENT_SHADER));
        if(!stage_sources[2].str().empty()) shaders.push_back(compile_shader(share, stage_sources[2].str(), GL_COMPUTE_SHADER));
        if(!stage_sources[3].str().empty()) shaders.push_back(compile_shader(share, stage_sources[3].str(), GL_GEOMETRY_SHADER));
        if(!stage_sources[4].str().empty()) shaders.push_back(compile_shader(share, stage_sources[4].str(), GL_TESS_CONTROL_SHADER));
        if(!stage_sources[5].str().empty()) shaders.push_back(compile_shader(share, stage_sources[5].str(), GL_TESS_EVALUATION_SHADER));

        const GLuint program = link_program(shaders);

        for(const GLuint shader : shaders)
        {
            if(shader != 0) glDeleteShader(shader);
        }

        return program;
    }

    void shader::load_uniform_locations()
    {
        int count = 0;
        constexpr std::array<GLenum, 1> properties = {GL_NAME_LENGTH};
        std::array<GLint, 1> name_length{};
        std::string name_buf;

        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

        for(int i = 0; i < count; i++)
        {
            glGetProgramResourceiv(program, GL_UNIFORM, i, 1, properties.data(), 1, nullptr, name_length.data());
            name_buf.resize(name_length[0] - 1);
            glGetProgramResourceName(program, GL_UNIFORM, i, name_length[0], nullptr, name_buf.data());
            const int location = glGetUniformLocation(program, name_buf.data());
            uniform_locations[name_buf] = location;
        }
    }

#define TRY_SET_UNIFORM(setter) if(const auto location = uniform_locations.find(name); location != uniform_locations.end()) { setter; } else logger::message_warn("Failed to set uniform '" + name + "'", logger_tag);

    void shader::set_uniform(const std::string& name, const bool value) const {
        TRY_SET_UNIFORM(glProgramUniform1i(program, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const bvec2 value) const {
        TRY_SET_UNIFORM(glProgramUniform2i(program, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const bvec3 value) const {
        TRY_SET_UNIFORM(glProgramUniform3i(program, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const bvec4 value) const {
        TRY_SET_UNIFORM(glProgramUniform4i(program, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const float value) const {
        TRY_SET_UNIFORM(glProgramUniform1f(program, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const vec2 value) const {
        TRY_SET_UNIFORM(glProgramUniform2f(program, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const vec3 value) const {
        TRY_SET_UNIFORM(glProgramUniform3f(program, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const vec4 value) const {
        TRY_SET_UNIFORM(glProgramUniform4f(program, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const double value) const {
        TRY_SET_UNIFORM(glProgramUniform1d(program, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const dvec2 value) const {
        TRY_SET_UNIFORM(glProgramUniform2d(program, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const dvec3& value) const {
        TRY_SET_UNIFORM(glProgramUniform3d(program, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const dvec4& value) const {
        TRY_SET_UNIFORM(glProgramUniform4d(program, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const int value) const {
        TRY_SET_UNIFORM(glProgramUniform1i(program, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const ivec2 value) const {
        TRY_SET_UNIFORM(glProgramUniform2i(program, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const ivec3 value) const {
        TRY_SET_UNIFORM(glProgramUniform3i(program, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const ivec4 value) const {
        TRY_SET_UNIFORM(glProgramUniform4i(program, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const unsigned int value) const {
        TRY_SET_UNIFORM(glProgramUniform1ui(program, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const uvec2 value) const {
        TRY_SET_UNIFORM(glProgramUniform2ui(program, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const uvec3 value) const {
        TRY_SET_UNIFORM(glProgramUniform3ui(program, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const uvec4 value) const {
        TRY_SET_UNIFORM(glProgramUniform4ui(program, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, mat2 value) const {
        TRY_SET_UNIFORM(glProgramUniformMatrix2fv(program, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string& name, mat3 value) const {
        TRY_SET_UNIFORM(glProgramUniformMatrix3fv(program, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string& name, mat4 value) const {
        TRY_SET_UNIFORM(glProgramUniformMatrix4fv(program, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_sampler(const std::string& name, const int texture, const int slot) const {
        glBindTextureUnit(slot, texture);
        TRY_SET_UNIFORM(glProgramUniform1i(program, location->second, slot));
    }
}
