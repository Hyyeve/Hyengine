#pragma once

#include "graphics.hpp"
#include "../common/math/interpolatable.hpp"
#include "../common/math/frustum.hpp"


namespace hyengine
{
    class camera
    {
    public:
        explicit camera(const glm::vec3& position, const glm::quat& rotation);

        explicit camera(glm::vec3 position) : camera(position, glm::quat(1, 0, 0, 0))
        {
        }

        camera() : camera(glm::vec3(0))
        {
        }

        void move_to(const glm::vec3& to);
        void move_to(f32 x, f32 y, f32 z);
        void move_by(const glm::vec3& delta);
        void move_by(f32 delta_x, f32 delta_y, f32 delta_z);
        void move_relative(const glm::vec3& delta);
        void move_relative(f32 delta_horizontal, f32 delta_vertical, f32 delta_axial);
        void move_planar(const glm::vec3& delta);
        void move_planar(f32 delta_horizontal, f32 delta_vertical, f32 delta_axial);

        void rotate_to(const glm::quat& to);
        void rotate_by(const glm::quat& delta);

        void zoom_by(f32 percent);
        void zoom_to(f32 percent);
        void zoom_focus_relative(const glm::vec3& target);
        void zoom_focus_absolute(const glm::dvec3& target);

        void set_perspective(f32 fov, f32 aspect, f32 near, f32 far);
        void set_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

        void refresh(f32 interpolation);
        void start_update();

        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec3& point, const render_viewport screen) const;
        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec2 point, const render_viewport screen) const;
        [[nodiscard]] glm::vec3 world_to_screen(const glm::vec3& point, const render_viewport screen) const;

        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec3& point) const;
        [[nodiscard]] glm::vec3 screen_to_world(glm::vec2 point) const;
        [[nodiscard]] glm::vec3 world_to_screen(const glm::vec3& point) const;

        [[nodiscard]] const glm::mat4& get_view() const;
        [[nodiscard]] const glm::mat4& get_projection() const;

        [[nodiscard]] glm::dvec3 get_position(const f32 interpolation) const;
        [[nodiscard]] glm::quat get_rotation(const f32 interpolation) const;
        [[nodiscard]] glm::vec3 get_rotation_radians(f32 interpolation) const;

        [[nodiscard]] glm::vec3 get_forward_dir() const;
        [[nodiscard]] glm::vec3 get_up_dir() const;
        [[nodiscard]] glm::vec3 get_right_dir() const;

        [[nodiscard]] bool frustum_visible(glm::dvec3 sphere_pos, f32 sphere_radius) const;
        [[nodiscard]] frustum get_frustum() const;

    private:
        interpolatable<glm::dvec3> position = glm::dvec3(0);
        interpolatable<glm::quat> rotation = glm::quat(0, 0, 0, 1);

        glm::vec3 forward = glm::vec3(0);
        glm::vec3 up = glm::vec3(0, 0, 0);
        glm::vec3 right = glm::vec3(0);

        interpolatable<glm::dvec3> zoom_target = glm::dvec3(0);
        interpolatable<f32> zoom_percent = 1.0f;

        glm::mat4 view_matrix = glm::mat4();
        glm::mat4 projection_matrix = glm::mat4();
        frustum cam_frustum = frustum();
    };
}
