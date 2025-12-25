#include "shader.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <tracy/Tracy.hpp>

#include "../../core/file_io.hpp"

namespace hyengine
{
    using namespace glm;


    shader::shader(const std::string_view& asset_id) noexcept : asset_id(asset_id), binary_asset_id(get_binary_asset_id(this->asset_id)) {}

    shader::~shader()
    {
        deallocate();
    }

    bool shader::allocate()
    {
        ZoneScoped;
        if (program_id != 0)
        {
            //Standard behaviour with shared shader instances
            log_warn(logger_tags::GRAPHICS, "Shader ", asset_id, " already loaded");
            return true;
        }

        program_id = load_program(asset_id, binary_asset_id);

        if (program_id == 0)
        {
            log_error(logger_tags::GRAPHICS, hyengine::stringify("Shader initialization for ", asset_id, " failed!"));
            return false;
        }

        load_interface_locations();
        log_debug(logger_tags::GRAPHICS, hyengine::stringify("Loaded shader ", asset_id, " and located ", uniform_locations.size(), " uniforms"));
        return true;
    }

    void shader::deallocate()
    {
        if (program_id == 0) return;

        glDeleteProgram(program_id);

        program_id = 0;
        uniform_locations.clear();
        log_debug(logger_tags::GRAPHICS, "Unloaded shader '", asset_id, "'");
    }

    bool shader::reallocate()
    {
        ZoneScoped;
        const bool in_use = active();
        deallocate();
        const bool allocated = allocate();
        if (!allocated) return false;
        if (in_use) use();
        return true;
    }

    void shader::clear_all_binary_caches()
    {
        delete_asset_directory(get_binary_asset_id("dummy"));
    }

    void shader::clear_binary_cache() const
    {
        delete_asset(binary_asset_id);
    }

    void shader::use() const
    {
        glUseProgram(program_id);
    }

    bool shader::valid() const
    {
        return program_id != 0;
    }

    bool shader::active() const
    {
        i32 current_program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        return current_program == program_id;
    }

    std::string shader::get_asset_id() const
    {
        return asset_id;
    }

    i32 shader::get_storage_block_binding(const std::string_view& name)
    {
        const std::string name_str = std::string(name);
        return storage_block_bindings.contains(name_str) ? storage_block_bindings[name_str].binding : -1;
    }

    void shader::set_storage_block_binding(const std::string_view& name, const i32 binding)
    {
        const std::string name_str = std::string(name);
        if (!storage_block_bindings.contains(name_str))
        {
            log_warn(logger_tags::GRAPHICS, "Failed to set shader storage block binding for '", name_str, "'`");
            return;
        }

        glShaderStorageBlockBinding(program_id, storage_block_bindings[name_str].location, binding);
    }

    i32 shader::get_uniform_block_binding(const std::string_view& name)
    {
        const std::string name_str = std::string(name);
        return uniform_block_bindings.contains(name_str) ? uniform_block_bindings[name_str].binding : -1;
    }

    void shader::set_uniform_block_binding(const std::string_view& name, const i32 binding)
    {
        const std::string name_str = std::string(name);
        if (!uniform_block_bindings.contains(name_str))
        {
            log_warn(logger_tags::GRAPHICS, "Failed to set shader uniform block binding for '", name_str, "'`");
            return;
        }

        glUniformBlockBinding(program_id, uniform_block_bindings[name_str].location, binding);
    }

    void log_shader_compile_info(const GLuint shader)
    {
        i32 log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string log;
        log.resize(max(log_length, 0));

        glGetShaderInfoLog(shader, log_length, nullptr, log.data());

        log.insert(0, "\n ---------- Failed to compile shader ---------- \n");
        log.insert(log.length() - 1, " --------------------------------------------- ");

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, log.length(), log.data());
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
            log_shader_compile_info(shader);
            return 0;
        }

        return shader;
    }

    void log_program_info(const std::string_view header, const GLuint program)
    {
        i32 log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        std::string log;
        log.resize(max(log_length, 0));

        glGetProgramInfoLog(program, log_length, nullptr, log.data());

        log.insert(0, header);
        log.insert(log.length() - 1, " -------------------------------------------- ");

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, log.length(), log.data());
    }

    bool validate_program(const GLuint program)
    {
        GLint success = GL_TRUE;

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &success);

        if (success != GL_TRUE)
        {
            log_program_info("\n -------- Failed to validate program -------- \n", program);
            return false;
        }
        return true;
    }

    GLuint shader::load_program(const std::string_view& asset_id, const std::string_view& binary_asset_id)
    {
        ZoneScoped;
        log_info(logger_tags::GRAPHICS, "Loading shader '", asset_id, "'.");
        if (!asset_exists(asset_id))
        {
            log_warn(logger_tags::GRAPHICS, "Shader asset '", asset_id, "' does not exist!");
            return 0;
        }

        const GLuint binary_program = load_binary_program(asset_id);
        if (binary_program != 0)
        {
            log_debug(logger_tags::GRAPHICS, "Found cached binary for '", asset_id, "'.");
            return binary_program;
        }

        log_debug(logger_tags::GRAPHICS, "Cached binary not found (or invalid) for '", asset_id, "'. Building shader.");
        const std::string raw_text = load_asset_text(asset_id);
        std::istringstream text(raw_text);

        std::string line;
        std::array<std::stringstream, 7> stage_sources {};

        i32 current_line_type = -1;
        while (getline(text, line))
        {
            const bool did_change = update_line_type(line, &current_line_type);
            if (did_change || current_line_type == -1) continue; //Skip including the type define lines
            stage_sources[current_line_type] << line << '\n';
        }

        const std::string share = stage_sources[6].str();

        std::vector<GLuint> shaders;
        {
            if (!stage_sources[0].str().empty()) shaders.push_back(compile_shader(share, stage_sources[0].str(), GL_VERTEX_SHADER));
            if (!stage_sources[1].str().empty()) shaders.push_back(compile_shader(share, stage_sources[1].str(), GL_FRAGMENT_SHADER));
            if (!stage_sources[2].str().empty()) shaders.push_back(compile_shader(share, stage_sources[2].str(), GL_COMPUTE_SHADER));
            if (!stage_sources[3].str().empty()) shaders.push_back(compile_shader(share, stage_sources[3].str(), GL_GEOMETRY_SHADER));
            if (!stage_sources[4].str().empty()) shaders.push_back(compile_shader(share, stage_sources[4].str(), GL_TESS_CONTROL_SHADER));
            if (!stage_sources[5].str().empty()) shaders.push_back(compile_shader(share, stage_sources[5].str(), GL_TESS_EVALUATION_SHADER));
        }

        const GLuint program = link_program(shaders);

        for (const GLuint shader : shaders)
        {
            //If we wanted to be super efficient about shader compilation, we should really keep these around for re-use.
            if (shader != 0)
            {
                glDeleteShader(shader);
            }
        }

        save_binary_program(asset_id, program);

        return program;
    }

    GLuint shader::link_program(const std::vector<GLuint>& shaders)
    {
        const GLuint program = glCreateProgram();
        for (const GLuint shader : shaders)
        {
            if (shader != 0)
            {
                glAttachShader(program, shader);
            }
        }

        glLinkProgram(program);

        GLint success = GL_TRUE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            log_program_info("\n ---------- Failed to link program ---------- \n", program);
            return 0;
        }

        return validate_program(program) ? program : 0;
    }

    std::string shader::get_binary_asset_id(const std::string_view& normal_asset_id)
    {
        return hyengine::stringify(binary_cache_directory, get_asset_name(normal_asset_id));
    }

    GLuint shader::load_binary_program(const std::string_view& asset_id)
    {
        ZoneScoped;
        const std::string binary_asset_id = get_binary_asset_id(asset_id);
        if (!asset_exists(binary_asset_id)) return 0;
        const std::vector<u8> binary_data = load_asset_bytes(binary_asset_id);

        const void* header_pointer = binary_data.data();
        const void* data_pointer = static_cast<const u8*>(header_pointer) + sizeof(binary_cache_header);
        const binary_cache_header header = *static_cast<const binary_cache_header*>(header_pointer);

        const u32 asset_hash = string_hash(get_asset_path(asset_id).string());
        if (asset_hash != header.asset_hash) return 0;

        const GLuint program = glCreateProgram();
        glProgramBinary(program, header.format, data_pointer, binary_data.size() - sizeof(binary_cache_header));

        return validate_program(program) ? program : 0;
    }

    void shader::save_binary_program(const std::string_view& asset_id, const GLuint program)
    {
        ZoneScoped;
        const std::string binary_asset_id = get_binary_asset_id(asset_id);

        if (program == 0) return;

        GLint buffer_size = 0;
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &buffer_size);

        std::vector<u8> binary_data(buffer_size + sizeof(binary_cache_header));
        void* pointer = binary_data.data();

        GLint written_length = 0;
        GLenum written_format = 0;
        const u32 asset_hash = string_hash(get_asset_path(asset_id).string());

        glGetProgramBinary(program, buffer_size, &written_length, &written_format, static_cast<u8*>(pointer) + sizeof(binary_cache_header));
        static_cast<binary_cache_header*>(pointer)[0] = binary_cache_header {written_format, asset_hash };

        if (written_length != buffer_size - sizeof(binary_cache_header))
        {
            binary_data.resize(written_length + sizeof(binary_cache_header));
        }

        const bool success = save_raw_asset(binary_asset_id, binary_data.data(), binary_data.size());
        if (!success)
        {
            log_warn(logger_tags::GRAPHICS, "Failed to save binary cache for shader '", asset_id, "'.");
        }
    }

    bool shader::update_line_type(const std::string_view line, i32* line_type)
    {
        const std::string type = ascii_to_lower(find_directive(line, "shader_type"));
        if (type.empty()) return false;
        switch (string_hash(type))
        {
            case string_hash("vert"):
            case string_hash("vertex"):
            {
                log_info(logger_tags::GRAPHICS, "Found vertex program.");
                *line_type = 0;
                break;
            }
            case string_hash("frag"):
            case string_hash("fragment"):
            {
                log_info(logger_tags::GRAPHICS, "Found fragment program.");
                *line_type = 1;
                break;
            }
            case string_hash("comp"):
            case string_hash("compute"):
            {
                log_info(logger_tags::GRAPHICS, "Found compute program.");
                *line_type = 2;
                break;
            }
            case string_hash("geom"):
            case string_hash("geometry"):
            {
                log_info(logger_tags::GRAPHICS, "Found geometry program.");
                *line_type = 3;
                break;
            }
            case string_hash("tess_control"):
            {
                log_info(logger_tags::GRAPHICS, "Found tessellation control program.");
                *line_type = 4;
                break;
            }
            case string_hash("tess_eval"):
            {
                log_info(logger_tags::GRAPHICS, "Found tessellation evaluation program.");
                *line_type = 5;
                break;
            }
            case string_hash("share"):
            case string_hash("shared"):
            {
                log_info(logger_tags::GRAPHICS, "Found shared program block.");
                *line_type = 6;
                break;
            }
            default: break;
        }

        return true;
    }

    void shader::load_interface_locations()
    {
        i32 uniform_count = 0;
        i32 ssbo_count = 0;
        i32 ubo_count = 0;
        i32 atcbo_count = 0;

        constexpr GLenum name_length_prop = GL_NAME_LENGTH;
        constexpr GLenum buffer_binding_prop = GL_BUFFER_BINDING;
        GLint name_length = 0;
        std::string name_buf;

        glGetProgramInterfaceiv(program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);
        glGetProgramInterfaceiv(program_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &ssbo_count);
        glGetProgramInterfaceiv(program_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &ubo_count);
        glGetProgramInterfaceiv(program_id, GL_ATOMIC_COUNTER_BUFFER, GL_ACTIVE_RESOURCES, &atcbo_count);

        for (u32 index = 0; index < uniform_count; index++)
        {
            glGetProgramResourceiv(program_id, GL_UNIFORM, index, 1, &name_length_prop, 1, nullptr, &name_length);
            name_buf.resize(name_length - 1);

            glGetProgramResourceName(program_id, GL_UNIFORM, index, name_length, nullptr, name_buf.data());
            const i32 location = glGetUniformLocation(program_id, name_buf.data());
            uniform_locations[name_buf] = location;

            if (location != index) log_debug(logger_tags::GRAPHICS, "Uniform location found that doesn't match index");
        }

        for (i32 index = 0; index < ssbo_count; index++)
        {
            i32 binding = 0;
            glGetProgramResourceiv(program_id, GL_SHADER_STORAGE_BLOCK, index, 1, &name_length_prop, 1, nullptr, &name_length);
            name_buf.resize(name_length - 1);

            glGetProgramResourceName(program_id, GL_SHADER_STORAGE_BLOCK, index, name_length, nullptr, name_buf.data());
            glGetProgramResourceiv(program_id, GL_SHADER_STORAGE_BLOCK, index, 1, &buffer_binding_prop, 1, nullptr, &binding);
            storage_block_bindings[name_buf] = {index, binding};
        }

        for (i32 index = 0; index < ubo_count; index++)
        {
            i32 binding = 0;
            glGetProgramResourceiv(program_id, GL_UNIFORM_BLOCK, index, 1, &name_length_prop, 1, nullptr, &name_length);
            name_buf.resize(name_length - 1);

            glGetProgramResourceName(program_id, GL_UNIFORM_BLOCK, index, name_length, nullptr, name_buf.data());
            glGetProgramResourceiv(program_id, GL_UNIFORM_BLOCK, index, 1, &buffer_binding_prop, 1, nullptr, &binding);
            uniform_block_bindings[name_buf] = {index, binding};
        }
    }

    #define TRY_SET_UNIFORM(setter) if(const auto location = uniform_locations.find(std::string(name)); location != uniform_locations.end()) { setter; } else if(valid()) { log_warn(logger_tags::GRAPHICS, "Failed to set uniform '",  name, "'"); }

    void shader::set_uniform(const std::string_view& name, const bool value)
    {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string_view& name, const bvec2 value)
    {
        TRY_SET_UNIFORM(glProgramUniform2i(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string_view& name, const bvec3 value)
    {
        TRY_SET_UNIFORM(glProgramUniform3i(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string_view& name, const bvec4 value)
    {
        TRY_SET_UNIFORM(glProgramUniform4i(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string_view& name, const f32 value)
    {
        TRY_SET_UNIFORM(glProgramUniform1f(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string_view& name, const vec2 value)
    {
        TRY_SET_UNIFORM(glProgramUniform2f(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string_view& name, const vec3 value)
    {
        TRY_SET_UNIFORM(glProgramUniform3f(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string_view& name, const vec4 value)
    {
        TRY_SET_UNIFORM(glProgramUniform4f(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string_view& name, const f64 value)
    {
        TRY_SET_UNIFORM(glProgramUniform1d(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string_view& name, const dvec2 value)
    {
        TRY_SET_UNIFORM(glProgramUniform2d(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string_view& name, const dvec3& value)
    {
        TRY_SET_UNIFORM(glProgramUniform3d(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string_view& name, const dvec4& value)
    {
        TRY_SET_UNIFORM(glProgramUniform4d(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string_view& name, const i32 value)
    {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string_view& name, const ivec2 value)
    {
        TRY_SET_UNIFORM(glProgramUniform2i(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string_view& name, const ivec3 value)
    {
        TRY_SET_UNIFORM(glProgramUniform3i(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string_view& name, const ivec4 value)
    {
        TRY_SET_UNIFORM(glProgramUniform4i(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string_view& name, const u32 value)
    {
        TRY_SET_UNIFORM(glProgramUniform1ui(program_id, location->second, value))
    }

    void shader::set_uniform(const std::string_view& name, const uvec2 value)
    {
        TRY_SET_UNIFORM(glProgramUniform2ui(program_id, location->second, value.x, value.y))
    }

    void shader::set_uniform(const std::string_view& name, const uvec3 value)
    {
        TRY_SET_UNIFORM(glProgramUniform3ui(program_id, location->second, value.x, value.y, value.z))
    }

    void shader::set_uniform(const std::string_view& name, const uvec4 value)
    {
        TRY_SET_UNIFORM(glProgramUniform4ui(program_id, location->second, value.x, value.y, value.z, value.w))
    }

    void shader::set_uniform(const std::string_view& name, mat2 value)
    {
        TRY_SET_UNIFORM(glProgramUniformMatrix2fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string_view& name, mat3 value)
    {
        TRY_SET_UNIFORM(glProgramUniformMatrix3fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_uniform(const std::string_view& name, mat4 value)
    {
        TRY_SET_UNIFORM(glProgramUniformMatrix4fv(program_id, location->second, 1, GL_FALSE, glm::value_ptr(value)))
    }

    void shader::set_sampler_slot(const std::string_view& name, const i32 slot)
    {
        TRY_SET_UNIFORM(glProgramUniform1i(program_id, location->second, slot));
    }
}
