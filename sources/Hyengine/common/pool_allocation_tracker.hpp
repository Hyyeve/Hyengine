#pragma once
#include <list>

namespace hyengine
{
    class pool_allocation_tracker
    {
    public:
        explicit pool_allocation_tracker(const unsigned int size);

        bool try_allocate(const unsigned int size, unsigned int& address);
        void deallocate(const unsigned int address);
        [[nodiscard]] unsigned int get_last_used_address() const;
        void resize(const unsigned int size);
        void clear();

    private:
        [[nodiscard]] bool find_free(const unsigned int size, unsigned int& start, unsigned int& block_idx, bool& is_end_allocation) const;

        struct allocation_block
        {
            unsigned int start;
            unsigned int size;
        };

        std::list<allocation_block> allocations{};
        unsigned int total_pool_size;
        unsigned int remaining_available_size;
    };
}
