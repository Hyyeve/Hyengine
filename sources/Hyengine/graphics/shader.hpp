#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../library/gl.hpp"
#include "../library/glm.hpp"
#include "../core/logger.hpp"


namespace hyengine::graphics {

    using namespace hyengine;

    class shader {
        static GLuint load_program(const std::string& asset_id);
        static GLuint link_program(const std::vector<GLuint>& shaders);

    public:
        explicit shader(std::string  asset_id, bool preload);
        explicit shader(std::string  asset_id) noexcept;

        shader(shader&& other) noexcept = delete;
        shader(const shader& other) noexcept = delete;
        shader& operator=(const shader& other) noexcept = delete;
        shader& operator=(shader&& other) noexcept = delete;

        ~shader();
        void use();
        [[nodiscard]] bool valid() const;
        [[nodiscard]] bool active() const;

        void set_uniform(const std::string& name, const bool value) const;
        void set_uniform(const std::string& name, const glm::bvec2 value) const;
        void set_uniform(const std::string& name, const glm::bvec3 value) const;
        void set_uniform(const std::string& name, const glm::bvec4 value) const;

        void set_uniform(const std::string& name, const float value) const;
        void set_uniform(const std::string& name, const glm::vec2 value) const;
        void set_uniform(const std::string& name, const glm::vec3 value) const;
        void set_uniform(const std::string& name, const glm::vec4 value) const;

        void set_uniform(const std::string& name, const double value) const;
        void set_uniform(const std::string& name, const glm::dvec2 value) const;
        void set_uniform(const std::string& name, const glm::dvec3& value) const;
        void set_uniform(const std::string& name, const glm::dvec4& value) const;

        void set_uniform(const std::string& name, const int value) const;
        void set_uniform(const std::string& name, const glm::ivec2 value) const;
        void set_uniform(const std::string& name, const glm::ivec3 value) const;
        void set_uniform(const std::string& name, const glm::ivec4 value) const;

        void set_uniform(const std::string& name, const unsigned int value) const;
        void set_uniform(const std::string& name, const glm::uvec2) const;
        void set_uniform(const std::string& name, const glm::uvec3 value) const;
        void set_uniform(const std::string& name, const glm::uvec4 value) const;

        void set_uniform(const std::string& name, const glm::mat2 value) const;
        void set_uniform(const std::string& name, const glm::mat3 value) const;
        void set_uniform(const std::string& name, const glm::mat4 value) const;

        void set_sampler(const std::string& name, int texture, int slot) const;


#define TRY_SET_UNIFORM(setter) if(const auto location = uniform_locations.find(name); location != uniform_locations.end()) { setter; } else logger::message_warn(logger::format("Failed to set uniform array '" + name + "'"), logger_tag);

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<bool, size> values) const {
            TRY_SET_UNIFORM(glUniform1iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::bvec2, size> values) const {
            TRY_SET_UNIFORM(glUniform2iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::bvec3, size> values) const {
            TRY_SET_UNIFORM(glUniform3iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::bvec4, size> values) const {
            TRY_SET_UNIFORM(glUniform4iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<float, size> values) const {
            TRY_SET_UNIFORM(glUniform1fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::vec2, size> values) const {
            TRY_SET_UNIFORM(glUniform2fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::vec3, size> values) const {
            TRY_SET_UNIFORM(glUniform3fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::vec4, size> values) const {
            TRY_SET_UNIFORM(glUniform4fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<double, size> values) const {
            TRY_SET_UNIFORM(glUniform1dv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::dvec2, size> values) const {
            TRY_SET_UNIFORM(glUniform2dv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::dvec3, size> values) const {
            TRY_SET_UNIFORM(glUniform3dv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::dvec4, size> values) const {
            TRY_SET_UNIFORM(glUniform4dv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<int, size> values) const {
            TRY_SET_UNIFORM(glUniform1iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::ivec2, size> values) const {
            TRY_SET_UNIFORM(glUniform2iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::ivec3, size> values) const {
            TRY_SET_UNIFORM(glUniform3iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::ivec4, size> values) const {
            TRY_SET_UNIFORM(glUniform4iv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<unsigned int, size> values) const {
            TRY_SET_UNIFORM(glUniform1uiv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::uvec2, size> values) const {
            TRY_SET_UNIFORM(glUniform2uiv(location->second, values.size(), values.data()))
        }


        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::uvec3, size> values) const {
            TRY_SET_UNIFORM(glUniform3uiv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::uvec4, size> values) const {
            TRY_SET_UNIFORM(glUniform4uiv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::mat2, size> values) const {
            TRY_SET_UNIFORM(glUniformMatrix2fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::mat3, size> values) const {
            TRY_SET_UNIFORM(glUniformMatrix3fv(location->second, values.size(), values.data()))
        }

        template <std::size_t size>
        void set_uniform(const std::string& name, std::array<glm::mat4, size> values) const {
            TRY_SET_UNIFORM(glUniformMatrix4fv(location->second, values.size(), values.data()))
        }

#undef TRY_SET_UNIFORM

    private:
        constexpr static std::string_view logger_tag = "Shader";

        std::unordered_map<std::string, int> uniform_locations;
        GLuint program = 0;
        bool initialized = false;
        std::string asset_id;

        void load_uniform_locations();
        void load();
        void unload();

    public:
        void reload();
    };
}
