#include "frustum.hpp"
#include <tracy/Tracy.hpp>
#include "../sized_numerics.hpp"

namespace hyengine
{
    void frustum::set_from_matrices(glm::mat4 projection, glm::mat4 view, glm::dvec3 origin)
    {
        ZoneScoped;
        const glm::mat4x4 mat = projection * view;

        //Near plane
        normals[0].x = mat[0][3];
        normals[0].y = mat[1][3];
        normals[0].z = mat[2][3];
        distances[0] = mat[3][3];

        //Down plane
        normals[1].x = mat[0][3] + mat[0][1];
        normals[1].y = mat[1][3] + mat[1][1];
        normals[1].z = mat[2][3] + mat[2][1];
        distances[1] = mat[3][3] + mat[3][1];

        //Left plane
        normals[2].x = mat[0][3] + mat[0][0];
        normals[2].y = mat[1][3] + mat[1][0];
        normals[2].z = mat[2][3] + mat[2][0];
        distances[2] = mat[3][3] + mat[3][0];

        //Right plane
        normals[3].x = mat[0][3] - mat[0][0];
        normals[3].y = mat[1][3] - mat[1][0];
        normals[3].z = mat[2][3] - mat[2][0];
        distances[3] = mat[3][3] - mat[3][0];

        //Up plane
        normals[4].x = mat[0][3] - mat[0][1];
        normals[4].y = mat[1][3] - mat[1][1];
        normals[4].z = mat[2][3] - mat[2][1];
        distances[4] = mat[3][3] - mat[3][1];

        //Far plane
        normals[5].x = mat[0][3] - mat[0][2];
        normals[5].y = mat[1][3] - mat[1][2];
        normals[5].z = mat[2][3] - mat[2][2];
        distances[5] = mat[3][3] - mat[3][2];

        for (i32 i = 0; i < 6; i++)
        {
            const f32 length = glm::length(normals[i]);
            normals[i] /= length;
            distances[i] /= length;
        }

        offset = origin;
    }

    void frustum::set_from_origin_and_window(glm::dvec3 origin, glm::vec3 forward, std::array<glm::dvec3, 4> window, f32 near, f32 far)
    {
        ZoneScoped;
        glm::vec3 top_left_dir = glm::normalize(origin - window[0]);
        glm::vec3 bottom_left_dir = glm::normalize(origin - window[1]);
        glm::vec3 bottom_right_dir = glm::normalize(origin - window[2]);
        glm::vec3 top_right_dir = glm::normalize(origin - window[3]);

        //Near plane
        normals[0] = normalize(forward);
        distances[0] = near;

        //Down plane
        normals[1] = glm::cross(bottom_left_dir, bottom_right_dir);
        distances[1] = 0;

        //Left plane
        normals[2] = glm::cross(top_left_dir, bottom_left_dir);
        distances[2] = 0;

        //Right plane
        normals[3] = glm::cross(bottom_right_dir, top_right_dir);
        distances[3] = 0;

        //Up plane
        normals[4] = glm::cross(top_right_dir, top_left_dir);
        distances[4] = 0;

        //Far plane
        normals[5] = -normals[0];
        distances[5] = far;

        offset = origin;
    }

    bool frustum::visible(glm::dvec3 sphere_pos, f32 sphere_radius) const
    {
        const glm::vec3 relative_pos = sphere_pos - offset;
        for (i32 i = 0; i < 6; i++)
        {
            if (glm::dot(relative_pos, normals[i]) + distances[i] + sphere_radius <= 0)
            {
                return false;
            }
        }

        return true;
    }
}
