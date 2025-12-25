#pragma once
#include <vector>

namespace hyengine
{
    ///Generates sequential IDs of a given type, with the ability to free IDs which will then be re-used. Useful for tracking items in a sparse array.
    template <typename id_type>
    class id_generator
    {
    public:
        ///Generates an ID. The id will either be an ID that was previously freed, or if no freed ids are available, the next ID in the sequence.
        id_type assign()
        {
            if (!freed_ids.empty())
            {
                id_type id = freed_ids.back();
                freed_ids.pop_back();
                return id;
            }

            id_type id = next_id;
            next_id = next_id + 1;
            return id;
        }

        ///Frees an ID. After an ID is freed, assigning a new ID will preferentially use a free ID.
        void free(id_type id)
        {
            freed_ids.push_back(id);
        }

        ///Clears internal state. IDs will start being generated from the start of the sequence again.
        void clear()
        {
            next_id = {};
            freed_ids.clear();
        }

    private:
        std::vector<id_type> freed_ids = {};
        id_type next_id = {};
    };
}
