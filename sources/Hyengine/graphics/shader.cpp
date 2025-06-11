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

    shader::shader(std::string asset_id) noexcept : asset_id(std::move(asset_id)), binary_asset_id(get_binary_asset_id(this->asset_id)) {}

    shader::~shader()
    {
        unload();
    }

    bool shader::valid() const
    {
        return program_id != 0;
    }

    bool shader::active() const
    {
        int current_program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        return current_program == program_id;
    }

    void shader::use()
    {
        glUseProgram(program_id);
    }

    void shader::load()
    {
        if(program_id != 0)
        {
            logger::warn(logger_tag, "Shader ", asset_id, " already loaded!");
            return;
        }

        program_id = load_program(asset_id, binary_asset_id);

        if(program_id != 0)
        {
            load_uniform_locations();
            logger::message_info(logger::format("Loaded shader ", asset_id, " and located ", uniform_locations.size(), " uniforms"), logger_tag);
        }
        else
        {
            logger::message_error(logger::format("Shader initialization for ", asset_id, " failed!"), logger_tag);
        }
    }

    void shader::unload() {
        if(program_id == 0) return;

        glDeleteProgram(program_id);

        program_id = 0;
        uniform_locations.clear();
    }

    void shader::reload()
    {
        const bool in_use = active();
        unload();
        load();
        if(in_use) use();
    }

    void shader::clear_binary_cache()
    {
        common::file_io::delete_asset_directory(get_binary_asset_id("shader:dummy"));
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

    void log_program_info(const std::string_view header, const GLuint program)
    {
        int log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        std::string log;
        log.resize(max(log_length, 0));

        glGetProgramInfoLog(program, log_length, nullptr, log.data());

        log.insert(0, header);
        log.insert(log.length() - 1, "\n ---------------------------------------- ");

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, log.length(), log.data());
    }

    bool validate_program(const GLuint program)
    {
        GLint success = GL_TRUE;
        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
        if (success != GL_TRUE)
        {
            log_program_info("\n ---------- Failed to validate program: \n", program);
            return false;
        }
        return true;
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
            log_program_info("\n ---------- Failed to link program: \n", program);
            return 0;
        }

        return validate_program(program) ? program : 0;
    }

    std::string shader::get_binary_asset_id(const std::string& normal_asset_id)
    {
        return logger::format(cache_directory, ":", common::file_io::get_asset_name(normal_asset_id));
    }

    static bool line_contains(const std::string_view line, const std::string_view str)
    {
        return line.find(str) != std::string::npos;
    }

    GLuint shader::load_program(const std::string& asset_id, const std::string& binary_asset_id) {

        if (!common::file_io::asset_exists(asset_id)) return 0;

        const GLuint binary_program = load_binary_program(binary_asset_id);
        if (binary_program != 0) return binary_program;

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

        save_binary_program(binary_asset_id, program);

        return program;
    }

    GLuint shader::load_binary_program(const std::string& asset_id)
    {
        if (!common::file_io::asset_exists(asset_id)) return 0;
        const std::vector<unsigned char> binary_data = common::file_io::read_raw_asset_bytes(asset_id);

        const void* header_pointer = binary_data.data();
        const void* data_pointer = static_cast<const unsigned char*>(header_pointer) + sizeof(GLenum);
        const GLenum format = *static_cast<const GLenum*>(header_pointer);

        const GLuint program = glCreateProgram();
        glProgramBinary(program, format, data_pointer, binary_data.size() - sizeof(GLenum));

        return validate_program(program) ? program : 0;
    }

    void shader::save_binary_program(const std::string& asset_id, const GLuint program)
    {
        if (program == 0) return;

        GLint buffer_size = 0;
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &buffer_size);

        std::vector<unsigned char> binary_data(buffer_size + sizeof(GLenum));
        void* pointer = binary_data.data();

        GLint written_length = 0;
        GLenum written_format = 0;

        glGetProgramBinary(program, buffer_size, &written_length, &written_format, static_cast<unsigned char*>(pointer) + sizeof(GLenum));
        static_cast<GLenum*>(pointer)[0] = written_format;

        if (written_length != buffer_size - sizeof(GLenum))
        {
            binary_data.resize(written_length + sizeof(GLenum));
        }

        common::file_io::save_raw_asset(asset_id, binary_data);
    }

    void shader::load_uniform_locations()
    {
        int count = 0;
        constexpr std::array<GLenum, 1> properties = {GL_NAME_LENGTH};
        std::array<GLint, 1> name_length{};
        std::string name_buf;

        glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &count);

        for(int i = 0; i < count; i++)
        {
            glGetProgramResourceiv(program_id, GL_UNIFORM, i, 1, properties.data(), 1, nullptr, name_length.data());
            name_buf.resize(name_length[0] - 1);
            glGetProgramResourceName(program_id, GL_UNIFORM, i, name_length[0], nullptr, name_buf.data());
            const int location = glGetUniformLocation(program_id, name_buf.data());
            uniform_locations[name_buf] = location;
        }
    }

#define TRY_SET_UNIFORM(setter) if(const auto location = uniform_locations.find(name); location != uniform_locations.end()) { setter; } else logger::message_warn("Failed to set uniform '" + name + "'", logger_tag);

    void shader::set_uniform(const std::string& name, const bool value) {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const bvec2 value) {
        TRY_SET_UNIFORM(glProgramUniform2i(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const bvec3 value) {
        TRY_SET_UNIFORM(glProgramUniform3i(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const bvec4 value) {
        TRY_SET_UNIFORM(glProgramUniform4i(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const float value) {
        TRY_SET_UNIFORM(glProgramUniform1f(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const vec2 value) {
        TRY_SET_UNIFORM(glProgramUniform2f(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const vec3 value) {
        TRY_SET_UNIFORM(glProgramUniform3f(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const vec4 value) {
        TRY_SET_UNIFORM(glProgramUniform4f(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const double value) {
        TRY_SET_UNIFORM(glProgramUniform1d(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const dvec2 value) {
        TRY_SET_UNIFORM(glProgramUniform2d(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const dvec3& value) {
        TRY_SET_UNIFORM(glProgramUniform3d(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const dvec4& value) {
        TRY_SET_UNIFORM(glProgramUniform4d(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const int value) {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const ivec2 value) {
        TRY_SET_UNIFORM(glProgramUniform2i(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const ivec3 value) {
        TRY_SET_UNIFORM(glProgramUniform3i(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const ivec4 value) {
        TRY_SET_UNIFORM(glProgramUniform4i(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, const unsigned int value) {
        TRY_SET_UNIFORM(glProgramUniform1ui(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string& name, const uvec2 value) {
        TRY_SET_UNIFORM(glProgramUniform2ui(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string& name, const uvec3 value) {
        TRY_SET_UNIFORM(glProgramUniform3ui(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string& name, const uvec4 value) {
        TRY_SET_UNIFORM(glProgramUniform4ui(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string& name, mat2 value) {
        TRY_SET_UNIFORM(glProgramUniformMatrix2fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string& name, mat3 value) {
        TRY_SET_UNIFORM(glProgramUniformMatrix3fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string& name, mat4 value) {
        TRY_SET_UNIFORM(glProgramUniformMatrix4fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_sampler_slot(const std::string& name, const int slot) {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, slot));
    }
}
