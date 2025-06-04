#pragma once
#include <array>

#include "graphics.hpp"
#include "../common/interpolatable.hpp"


namespace hyengine::graphics {

    using namespace hyengine;

    class camera {
    public:
        explicit camera(glm::vec3 position) : camera(position, glm::vec3(0, -90, 0)) {}
        camera() : camera(glm::vec3(0)) {};
        camera(const glm::vec3 &position, const glm::vec3 &rotation);

        void move_to(const glm::vec3 &to);
        void move_to(float x, float y, float z);
        void move_by(const glm::vec3 &delta);
        void move_by(float deltaX, float deltaY, float deltaZ);
        void move_relative(const glm::vec3 &delta);
        void move_relative(float deltaHorizontal, float deltaVertical, float deltaAxial);
        void move_planar(const glm::vec3 &delta);
        void move_planar(float deltaHorizontal, float deltaVertical, float deltaAxial);

        void rotate_to(const glm::vec3 &to);
        void rotate_to(const glm::vec2 to);
        void rotate_by(const glm::vec3 &delta);
        void rotate_by(const glm::vec2 delta);

        void zoom_by(float percent);
        void zoom_to(float percent);
        void zoom_focus_relative(const glm::vec3 &target);
        void zoom_focus_absolute(const glm::dvec3& target);

        void set_perspective(float fov, float aspect, float near, float far);
        void set_orthographic(float left, float right, float bottom, float top, float near, float far);

        void refresh(float interpolation);
        void start_update();

        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec3 &point, const viewport screen) const;
        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec2 point, const viewport screen) const;
        [[nodiscard]] glm::vec3 world_to_screen(const glm::vec3 &point, const viewport screen) const;

        [[nodiscard]] glm::vec3 screen_to_world(const glm::vec3 &point) const;
        [[nodiscard]] glm::vec3 screen_to_world(glm::vec2 point) const;
        [[nodiscard]] glm::vec3 world_to_screen(const glm::vec3 &point) const;

        [[nodiscard]] const glm::mat4& get_view() const;
        [[nodiscard]] const glm::mat4& get_projection() const;

        [[nodiscard]] glm::dvec3 get_position(const float interpolation) const;
        [[nodiscard]] glm::vec3 get_rotation_degrees(float interpolation) const;
        [[nodiscard]] glm::vec3 get_rotation_radians() const;

        [[nodiscard]] glm::vec3 get_look_dir() const;
        [[nodiscard]] glm::vec3 get_forward_dir() const;
        [[nodiscard]] glm::vec3 get_up_dir() const;
        [[nodiscard]] glm::vec3 get_right_dir() const;

        [[nodiscard]] bool frustum_visible(glm::vec3 sphere_pos, float sphere_radius) const;


    private:
        struct frustum_planes
        {
            std::array<glm::vec3, 6> normals;
            std::array<float, 6> distances;
        };

        void update_frustum(double interpolation);

        common::interpolatable<glm::dvec3> position = glm::dvec3(0);
        glm::dvec3 constraint_size = glm::dvec3(INFINITY);
        common::interpolatable<glm::vec3> rotation_degrees = glm::vec3(0);
        glm::vec3 rotation_radians = glm::vec3(0);
        glm::vec3 look = glm::vec3(0);
        glm::vec3 forward = glm::vec3(0);
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::vec3(0);
        common::interpolatable<glm::vec3> zoom_target = glm::vec3(0);
        common::interpolatable<float> zoom_percent = 1.0f;
        glm::mat4 view_matrix = glm::mat4();
        glm::mat4 projection_matrix = glm::mat4();
        frustum_planes frustum;
    };

}
