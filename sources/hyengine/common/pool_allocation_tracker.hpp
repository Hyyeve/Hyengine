#pragma once
#include <list>

#include "sized_numerics.hpp"

namespace hyengine
{
    ///Simple memory pool allocation management. Allows allocating and freeing sized blocks within a given range.
    class pool_allocation_tracker
    {
    public:
        explicit pool_allocation_tracker(const u32 size);

        bool try_allocate(const u32 size, u32& address_out);
        void deallocate(const u32 address);
        [[nodiscard]] u32 get_last_used_address() const;
        void resize(const u32 size);
        void clear();

    private:
        [[nodiscard]] bool find_free(const u32 size, u32& start, u32& block_idx, bool& is_end_allocation) const;

        struct allocation_block
        {
            u32 start;
            u32 size;
        };

        std::list<allocation_block> allocations {};
        u32 total_pool_size;
        u32 remaining_available_size;
    };
}
