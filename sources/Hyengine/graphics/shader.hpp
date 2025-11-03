#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <tracy/Tracy.hpp>

#include "../library/gl.hpp"
#include "../library/glm.hpp"
#include "../core/logger.hpp"
#include "../common/sized_numerics.hpp"


namespace hyengine
{
    class shader
    {
    public:
        explicit shader(const std::string_view& asset_id) noexcept;

        shader(shader&& other) noexcept = delete;
        shader(const shader& other) noexcept = delete;
        shader& operator=(const shader& other) noexcept = delete;
        shader& operator=(shader&& other) noexcept = delete;

        ~shader();

        void load();
        void unload();
        void reload();

        static void clear_all_shader_caches();
        void clear_shader_cache() const;

        void use() const;

        [[nodiscard]] bool valid() const;
        [[nodiscard]] bool active() const;
        [[nodiscard]] std::string get_asset_id() const;

        void set_uniform(const std::string_view& name, const bool value);
        void set_uniform(const std::string_view& name, const glm::bvec2 value);
        void set_uniform(const std::string_view& name, const glm::bvec3 value);
        void set_uniform(const std::string_view& name, const glm::bvec4 value);

        void set_uniform(const std::string_view& name, const float value);
        void set_uniform(const std::string_view& name, const glm::vec2 value);
        void set_uniform(const std::string_view& name, const glm::vec3 value);
        void set_uniform(const std::string_view& name, const glm::vec4 value);

        void set_uniform(const std::string_view& name, const double value);
        void set_uniform(const std::string_view& name, const glm::dvec2 value);
        void set_uniform(const std::string_view& name, const glm::dvec3& value);
        void set_uniform(const std::string_view& name, const glm::dvec4& value);

        void set_uniform(const std::string_view& name, const i32 value);
        void set_uniform(const std::string_view& name, const glm::ivec2 value);
        void set_uniform(const std::string_view& name, const glm::ivec3 value);
        void set_uniform(const std::string_view& name, const glm::ivec4 value);

        void set_uniform(const std::string_view& name, const u32 value);
        void set_uniform(const std::string_view& name, const glm::uvec2);
        void set_uniform(const std::string_view& name, const glm::uvec3 value);
        void set_uniform(const std::string_view& name, const glm::uvec4 value);

        void set_uniform(const std::string_view& name, const glm::mat2 value);
        void set_uniform(const std::string_view& name, const glm::mat3 value);
        void set_uniform(const std::string_view& name, const glm::mat4 value);

        void set_sampler_slot(const std::string_view& name, i32 slot);


        #define TRY_SET_UNIFORM(setter) if(const auto location = uniform_locations.find(std::string(name)); location != uniform_locations.end()) { setter; } else log_warn(logger_tag, "Failed to set uniform array '", name, "'");

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<bool, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform1iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::bvec2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform2iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::bvec3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform3iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::bvec4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform4iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<float, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform1fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::vec2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform2fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::vec3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform3fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::vec4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform4fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<double, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform1dv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::dvec2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform2dv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::dvec3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform3dv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::dvec4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform4dv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<i32, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform1iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::ivec2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform2iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::ivec3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform3iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::ivec4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform4iv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<u32, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform1uiv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::uvec2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform2uiv(program_id, location->second, values.size(), values.data()))
        }


        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::uvec3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform3uiv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::uvec4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniform4uiv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::mat2, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniformMatrix2fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::mat3, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniformMatrix3fv(program_id, location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string_view& name, std::array<glm::mat4, size> values)
        {
            ZoneScoped;
            TRY_SET_UNIFORM(glProgramUniformMatrix4fv(program_id, location->second, values.size(), values.data()))
        }

        #undef TRY_SET_UNIFORM

    private:
        static GLuint load_program(const std::string_view& asset_id, const std::string_view& binary_asset_id);
        static GLuint link_program(const std::vector<GLuint>& shaders);
        static std::string get_binary_asset_id(const std::string_view& normal_asset_id);
        static GLuint load_binary_program(const std::string_view& asset_id);
        static void save_binary_program(const std::string_view& asset_id, const GLuint program);

        void load_uniform_locations();

        constexpr static std::string_view logger_tag = "Shader";
        constexpr static std::string_view cache_directory = "shader.bin";

        std::unordered_map<std::string, i32> uniform_locations;

        GLuint program_id = 0;
        std::string asset_id;
        std::string binary_asset_id;
    };
}
