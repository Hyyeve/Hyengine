#pragma once
#include <vector>

namespace hyengine::common {

    template <typename ID_TYPE>
    class id_generator {
    public:
        ID_TYPE assign()
        {
            if(!freed_ids.empty())
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
            freed_ids.push_back(id);
        }

        void clear()
        {
            next_id = {};
            freed_ids.clear();
        }

    private:
        std::vector<ID_TYPE> freed_ids = {};
        ID_TYPE next_id = {};
    };

}
