#include "camera.hpp"

namespace hyengine::graphics {

    using namespace hyengine;
    using namespace glm;

    camera::camera(const vec3& position, const vec3& rotation) : position(position), rotation(rotation) {
        refresh(0.0);
    }

    void camera::move_to(const vec3& to) {
        position = to;
    }

    void camera::move_to(const float x, const float y, const float z) {
        position = vec3(x, y, z);
    }

    void camera::move_by(const vec3& delta) {
        position += delta;
    }

    void camera::move_by(const float deltaX, const float deltaY, const float deltaZ) {
        position += vec3(deltaX, deltaY, deltaZ);
    }

    void camera::move_relative(const vec3& delta) {
        position += vec3(delta.x * right.x + delta.y * up.x + delta.z * forward.x, delta.x * right.y + delta.y * up.y + delta.z * forward.y, delta.x * right.z + delta.y * up.z + delta.z * forward.z);
    }

    void camera::move_planar(const vec3& delta) {
        position += vec3(delta.x * right.x + delta.y * up.x + delta.z * forward.x, delta.x * right.y + delta.y * up.y + delta.z * forward.y, delta.x * right.z + delta.y * up.z + delta.z * forward.z);
    }

    void camera::move_relative(const float deltaHorizontal, const float deltaVertical, const float deltaAxial) {
        position += vec3(deltaHorizontal * right.x + deltaVertical * up.x + deltaAxial * forward.x, deltaHorizontal * right.y + deltaVertical * up.y + deltaAxial * forward.y, deltaHorizontal * right.z + deltaVertical * up.z + deltaAxial * forward.z);
    }

    void camera::move_planar(const float deltaHorizontal, const float deltaVertical, const float deltaAxial) {
        position += vec3(deltaHorizontal * right.x + deltaVertical * up.x + deltaAxial * forward.x, deltaHorizontal * right.y + deltaVertical * up.y + deltaAxial * forward.y, deltaHorizontal * right.z + deltaVertical * up.z + deltaAxial * forward.z);
    }

    void camera::rotate_to(const glm::quat& to)
    {
        rotation = glm::quat(to);
    }

    void camera::rotate_by(const glm::quat& delta)
    {
        rotation *= delta;
    }

    void camera::zoom_by(const float percent) {
        zoom_percent += percent;
    }

    void camera::zoom_to(const float percent) {
        zoom_percent = percent;
    }

    void camera::zoom_focus_relative(const vec3& target) {
        zoom_target = target;
    }

    void camera::zoom_focus_absolute(const dvec3& target) {
        dvec3 exact = (target - position);
        zoom_target = vec3(exact.x, exact.y, exact.z);
    }

    void camera::set_perspective(const float fov, const float aspect, const float near, const float far) {
        projection_matrix = perspective(fov, aspect, near, far);
        zoom_target = vec3(0);
    }

    void camera::set_orthographic(const float left, const float right, const float bottom, const float top, const float near, const float far) {
        projection_matrix = ortho(left, right, bottom, top, near, far);
        zoom_target = vec3((left + right) / 2.0f, (bottom + top) / 2.0f, (near + far) / 2.0f);
    }

    void camera::refresh(const float interpolation) {

        glm::quat interpolated_rotation = rotation.interpolated(interpolation);
        const float interpolated_zoom_percent = zoom_percent.interpolated(interpolation);
        const vec3 relative_zoom_target = zoom_target.interpolated<double>(interpolation) - get_position(interpolation);

        forward = glm::vec3(0, 0, 1) * interpolated_rotation;
        up = glm::vec3(0, 1, 0) * interpolated_rotation;
        right = glm::vec3(1, 0, 0) * interpolated_rotation;

        view_matrix = toMat4(interpolated_rotation);

        if(interpolated_zoom_percent != 1.0f) { //zoom is probably going to be 1.0 a lot of the time
            const vec3 zoom_translation = relative_zoom_target;
            view_matrix = translate(view_matrix, zoom_translation);
            view_matrix = scale(view_matrix, vec3(interpolated_zoom_percent));
            view_matrix = translate(view_matrix, -zoom_translation);
        }

        frustum.set_from_matrices(projection_matrix, view_matrix, get_position(interpolation));
    }

    void camera::start_update() {
        position.stabilize();
        rotation.stabilize();
        zoom_percent.stabilize();
        zoom_target.stabilize();
    }

    const mat4& camera::get_view() const {
        return view_matrix;
    }

    const mat4& camera::get_projection() const {
        return projection_matrix;
    }

    dvec3 camera::get_position(const float interpolation) const {
        return position.interpolated<double>(interpolation);
    }

    glm::quat camera::get_rotation(const float interpolation) const
    {
        return rotation.interpolated(interpolation);
    }

    glm::vec3 camera::get_rotation_radians(const float interpolation) const
    {
        return eulerAngles(rotation.interpolated(interpolation));
    }

    vec3 camera::get_forward_dir() const{
        return forward;
    }

    vec3 camera::get_up_dir() const{
        return up;
    }

    vec3 camera::get_right_dir() const {
        return right;
    }

    bool camera::frustum_visible(glm::dvec3 sphere_pos, float sphere_radius) const
    {
        return frustum.visible(sphere_pos, sphere_radius);
    }

    common::frustum camera::get_frustum() const
    {
        return frustum;
    }

    vec3 camera::screen_to_world(const vec3& point, const viewport screen) const {
        return unProject(point, view_matrix, projection_matrix, vec4(screen.x_offset, screen.y_offset, screen.width, screen.height));
    }

    vec3 camera::screen_to_world(const vec2 point, const viewport screen) const {
        return screen_to_world(vec3(point, 0), screen);
    }

    vec3 camera::world_to_screen(const vec3& point, const viewport screen) const {
        return project(point, view_matrix, projection_matrix, vec4(screen.x_offset, screen.y_offset, screen.width, screen.height));
    }

    vec3 camera::screen_to_world(const vec3& point) const {
        return screen_to_world(point, get_viewport());
    }

    vec3 camera::screen_to_world(const vec2 point) const {
        return screen_to_world(vec3(point, 0));
    }

    vec3 camera::world_to_screen(const vec3& point) const {
        return world_to_screen(point, get_viewport());
    }
}


