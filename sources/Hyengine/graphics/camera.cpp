#include "camera.hpp"

namespace hyengine::graphics {

    using namespace hyengine;
    using namespace glm;

    camera::camera(const vec3& position, const vec3& rotation) : position(position), rotation_degrees(rotation) {
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
        position += vec3(delta.x * right.x + delta.y * up.x + delta.z * look.x, delta.x * right.y + delta.y * up.y + delta.z * look.y, delta.x * right.z + delta.y * up.z + delta.z * look.z);
    }

    void camera::move_planar(const vec3& delta) {
        position += vec3(delta.x * right.x + delta.y * up.x + delta.z * forward.x, delta.x * right.y + delta.y * up.y + delta.z * forward.y, delta.x * right.z + delta.y * up.z + delta.z * forward.z);
    }

    void camera::move_relative(const float deltaHorizontal, const float deltaVertical, const float deltaAxial) {
        position += vec3(deltaHorizontal * right.x + deltaVertical * up.x + deltaAxial * look.x, deltaHorizontal * right.y + deltaVertical * up.y + deltaAxial * look.y, deltaHorizontal * right.z + deltaVertical * up.z + deltaAxial * look.z);
    }

    void camera::move_planar(const float deltaHorizontal, const float deltaVertical, const float deltaAxial) {
        position += vec3(deltaHorizontal * right.x + deltaVertical * up.x + deltaAxial * forward.x, deltaHorizontal * right.y + deltaVertical * up.y + deltaAxial * forward.y, deltaHorizontal * right.z + deltaVertical * up.z + deltaAxial * forward.z);
    }

    void camera::rotate_to(const vec3& to) {
        rotation_degrees = to;
    }

    void camera::rotate_to(const vec2 to) {
        rotation_degrees = vec3(to.x, to.y, 0);
    }

    void camera::rotate_by(const vec3& delta) {
        rotation_degrees += delta;
    }

    void camera::rotate_by(const vec2 delta) {
        rotation_degrees += vec3(delta.x, delta.y, 0);
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

        vec3 interpolatedRotation = rotation_degrees.interpolated(interpolation);
        const float interpolatedZoomPercent = zoom_percent.interpolated(interpolation);
        const vec3 interpolatedZoomTarget = zoom_target.interpolated(interpolation);

        interpolatedRotation = vec3(fmod(interpolatedRotation.x, 360.0f), fmod(interpolatedRotation.y, 360.0f), fmod(interpolatedRotation.z, 360.0f));
        interpolatedRotation.x = max(min(interpolatedRotation.x, 89.0f), -89.0f);
        rotation_radians = radians(interpolatedRotation);

        look = vec3(cos(rotation_radians.y) * cos(rotation_radians.x), sin(rotation_radians.x), sin(rotation_radians.y) * cos(rotation_radians.x));
        look = normalize(look);
        right = normalize(cross(look, up));
        forward = -normalize(cross(right, up));
        view_matrix = lookAt(vec3(0), look, up);

        if(interpolatedZoomPercent != 1.0f) { //zoom is probably going to be 1.0 a lot of the time
            const vec3 zoomTranslation = interpolatedZoomTarget;
            view_matrix = translate(view_matrix, zoomTranslation);
            view_matrix = scale(view_matrix, vec3(interpolatedZoomPercent));
            view_matrix = translate(view_matrix, -zoomTranslation);
        }

        update_frustum(interpolation);
    }



    void camera::start_update() {
        position.stabilize();
        rotation_degrees.stabilize();
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

    vec3 camera::get_rotation_degrees(const float interpolation) const {
        return rotation_degrees.interpolated(interpolation);
    }

    vec3 camera::get_rotation_radians() const {
        return rotation_radians;
    }

    vec3 camera::get_look_dir() const {
        return look;
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

    bool camera::frustum_visible(glm::vec3 sphere_pos, float sphere_radius) const
    {
        for (int i = 0; i < 6; i++)
        {
            if (glm::dot(sphere_pos, frustum.normals[i]) + frustum.distances[i] + sphere_radius <= 0)
            {
                return false;
            }
        }

        return true;
    }

    void camera::update_frustum(const double interpolation)
    {
        const glm::mat4x4 mat = projection_matrix * translate(view_matrix, -static_cast<vec3>(get_position(interpolation)));

        //Near plane
        frustum.normals[0].x = mat[0][3];
        frustum.normals[0].y = mat[1][3];
        frustum.normals[0].z = mat[2][3];
        frustum.distances[0] = mat[3][3];

        //Down plane
        frustum.normals[1].x = mat[0][3] + mat[0][1];
        frustum.normals[1].y = mat[1][3] + mat[1][1];
        frustum.normals[1].z = mat[2][3] + mat[2][1];
        frustum.distances[1] = mat[3][3] + mat[3][1];

        //Left plane
        frustum.normals[2].x = mat[0][3] + mat[0][0];
        frustum.normals[2].y = mat[1][3] + mat[1][0];
        frustum.normals[2].z = mat[2][3] + mat[2][0];
        frustum.distances[2] = mat[3][3] + mat[3][0];

        //Right plane
        frustum.normals[3].x = mat[0][3] - mat[0][0];
        frustum.normals[3].y = mat[1][3] - mat[1][0];
        frustum.normals[3].z = mat[2][3] - mat[2][0];
        frustum.distances[3] = mat[3][3] - mat[3][0];

        //Up plane
        frustum.normals[4].x = mat[0][3] - mat[0][1];
        frustum.normals[4].y = mat[1][3] - mat[1][1];
        frustum.normals[4].z = mat[2][3] - mat[2][1];
        frustum.distances[4] = mat[3][3] - mat[3][1];

        //Far plane
        frustum.normals[5].x = mat[0][3] - mat[0][2];
        frustum.normals[5].y = mat[1][3] - mat[1][2];
        frustum.normals[5].z = mat[2][3] - mat[2][2];
        frustum.distances[5] = mat[3][3] - mat[3][2];

        for(int i = 0; i < 6; i++)
        {
            const float length = glm::length(frustum.normals[i]);
            frustum.normals[i] /= length;
            frustum.distances[i] /= length;
        }
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


