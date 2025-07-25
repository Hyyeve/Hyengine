#pragma once
#include <vector>
#include <tracy/Tracy.hpp>

namespace hyengine
{
    template <typename ID_TYPE>
    class id_generator
    {
    public:
        ID_TYPE assign()
        {
            ZoneScoped;
            if (!freed_ids.empty())
            {
                ID_TYPE id = freed_ids.back();
                freed_ids.pop_back();
                return id;
            }

            ID_TYPE id = next_id;
            next_id = next_id + 1;
            return id;
        }

        void free(ID_TYPE id)
        {
            ZoneScoped;
            freed_ids.push_back(id);
        }

        void clear()
        {
            ZoneScoped;
            next_id = {};
            freed_ids.clear();
        }

    private:
        std::vector<ID_TYPE> freed_ids = {};
        ID_TYPE next_id = {};
    };
}
