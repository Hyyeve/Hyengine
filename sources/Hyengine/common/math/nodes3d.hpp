#pragma once
#include "../../library/glm.hpp"

namespace hyengine::common {
    template<typename data_type>
    class nodes_3d
    {
    public:
        struct node_data
        {
            data_type data;

            node_data* x_left;
            node_data* x_right;
            node_data* y_up;
            node_data* y_down;
            node_data* z_back;
            node_data* z_front;
        };

        void insert(glm::ivec3 position, data_type data)
        {
            if (data_map.contains(position))
            {
                data_map[position].data = data;
                return;
            }

            node_data* left = data_map.contains(position + glm::ivec3(-1, 0, 0)) ? &data_map[position + glm::ivec3(-1, 0, 0)] : nullptr;
            node_data* right = data_map.contains(position + glm::ivec3(1, 0, 0)) ? &data_map[position + glm::ivec3(1, 0, 0)] : nullptr;
            node_data* down = data_map.contains(position + glm::ivec3(0, -1, 0)) ? &data_map[position + glm::ivec3(0, -1, 0)] : nullptr;
            node_data* up = data_map.contains(position + glm::ivec3(0, 1, 0)) ? &data_map[position + glm::ivec3(0, 1, 0)] : nullptr;
            node_data* back = data_map.contains(position + glm::ivec3(0, 0, -1)) ? &data_map[position + glm::ivec3(0, 0, -1)] : nullptr;
            node_data* front = data_map.contains(position + glm::ivec3(0, 0, 1)) ? &data_map[position + glm::ivec3(0, 0, 1)] : nullptr;

            node_data node = {
                data,
                left,
                right,
                up,
                down,
                back,
                front
            };

            data_map.emplace(position, node);
            node_data* inserted = &data_map[position];

            if (left != nullptr) left->x_right = inserted;
            if (right != nullptr) right->x_left = inserted;
            if (up != nullptr) up->y_down = inserted;
            if (down != nullptr) down->y_up = inserted;
            if (back != nullptr) back->z_front = inserted;
            if (front != nullptr) front->z_back = inserted;
        }

        void erase(glm::ivec3 position)
        {
            if (data_map.contains(position))
            {
                node_data& data = data_map[position];
                if (data.x_left != nullptr) data.x_left->x_right = nullptr;
                if (data.x_right != nullptr) data.x_right->x_left = nullptr;
                if (data.y_down != nullptr) data.y_down->y_up = nullptr;
                if (data.y_up != nullptr) data.y_up->y_down = nullptr;
                if (data.z_front != nullptr) data.z_front->z_back = nullptr;
                if (data.z_back != nullptr) data.z_back->z_front = nullptr;
                data_map.erase(position);
            }
        }

        node_data* lookup(glm::ivec3 position)
        {
            if (data_map.contains(position))
            {
                return &data_map[position];
            }

            return nullptr;
        }

        void clear()
        {
            data_map.clear();
        }

    private:
        std::unordered_map<glm::ivec3, node_data> data_map;
    };
}
