#pragma once

#include "../../library/gl.hpp"
#include "../../library/glm.hpp"

namespace hyengine::graphics {

    class texture_sampler_state {
    public:
        texture_sampler_state(const texture_sampler_state& other) = delete; //COPY CONSTRUCTOR
        texture_sampler_state(texture_sampler_state&& other) = delete; //MOVE CONSTRUCTOR
        texture_sampler_state& operator=(const texture_sampler_state& other) = delete; //COPY ASSIGNMENT
        texture_sampler_state& operator=(texture_sampler_state&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit texture_sampler_state() = default;
        ~texture_sampler_state();

        void allocate();
        void free();

        void set_border_color(const glm::vec4 col) const;

        void set_compare_func(const GLenum func) const;
        void set_compare_mode(const GLenum mode) const;

        void set_anisotropy_level(const float anisotropy) const;
        void set_downscale_filter(const GLenum filter) const;
        void set_upscale_filter(const GLenum filter) const;

        void set_lod_bias(const float bias) const;
        void set_min_lod(const int lod) const;
        void set_max_lod(const int lod) const;

        void set_texture_wrap_x(const GLenum wrap) const;
        void set_texture_wrap_y(const GLenum wrap) const;
        void set_texture_wrap_z(const GLenum wrap) const;

        void bind(const int slot) const;

        [[nodiscard]] GLuint get_id() const;

    private:
        GLuint gl_id = 0;
    };
}
