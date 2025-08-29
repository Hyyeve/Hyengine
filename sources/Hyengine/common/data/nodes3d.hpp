#pragma once
#include <tracy/Tracy.hpp>

#include "../../library/glm.hpp"

namespace hyengine
{
    template <typename data_type>
    class nodes_3d
    {
    public:
        struct node_data
        {
            data_type data;
            glm::ivec3 position;
            std::array<node_data*, 6> neighbours; //-X, +X, -Y, +Y, -Z, +Z
        };

        void insert(glm::ivec3 position, data_type data)
        {
            if (data_map.contains(position))
            {
                data_map[position].data = data;
                return;
            }

            node_data* neg_x = lookup(position + glm::ivec3(-1, 0, 0));
            node_data* pos_x = lookup(position + glm::ivec3(1, 0, 0));
            node_data* neg_y = lookup(position + glm::ivec3(0, -1, 0));
            node_data* pos_y = lookup(position + glm::ivec3(0, 1, 0));
            node_data* neg_z = lookup(position + glm::ivec3(0, 0, -1));
            node_data* pos_z = lookup(position + glm::ivec3(0, 0, 1));

            node_data node = {
                data,
                position,
                {neg_x, pos_x, neg_y, pos_y, neg_z, pos_z}
            };

            data_map.emplace(position, node);
            node_data* inserted = &data_map[position];

            if (pos_x != nullptr) pos_x->neighbours[0] = inserted;
            if (neg_x != nullptr) neg_x->neighbours[1] = inserted;
            if (pos_y != nullptr) pos_y->neighbours[2] = inserted;
            if (neg_y != nullptr) neg_y->neighbours[3] = inserted;
            if (pos_z != nullptr) pos_z->neighbours[4] = inserted;
            if (neg_z != nullptr) neg_z->neighbours[5] = inserted;
        }

        void erase(glm::ivec3 position)
        {
            if (data_map.contains(position))
            {
                node_data& data = data_map[position];
                if (data.neighbours[0] != nullptr) data.neighbours[0]->neighbours[1] = nullptr;
                if (data.neighbours[1] != nullptr) data.neighbours[1]->neighbours[0] = nullptr;
                if (data.neighbours[2] != nullptr) data.neighbours[2]->neighbours[3] = nullptr;
                if (data.neighbours[3] != nullptr) data.neighbours[3]->neighbours[2] = nullptr;
                if (data.neighbours[4] != nullptr) data.neighbours[4]->neighbours[5] = nullptr;
                if (data.neighbours[5] != nullptr) data.neighbours[5]->neighbours[4] = nullptr;
                data_map.erase(position);
            }
        }

        bool contains(glm::ivec3 position)
        {
            return data_map.contains(position);
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
