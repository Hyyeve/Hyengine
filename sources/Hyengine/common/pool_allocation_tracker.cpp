#include "pool_allocation_tracker.hpp"

#include <tracy/Tracy.hpp>

namespace hyengine
{
    pool_allocation_tracker::pool_allocation_tracker(const u32 size): total_pool_size(size), remaining_available_size(total_pool_size)
    {
    }

    bool pool_allocation_tracker::try_allocate(const u32 size, u32& address)
    {
        ZoneScoped;
        u32 block_idx;
        bool is_end_allocation;
        const bool has_space = find_free(size, address, block_idx, is_end_allocation);

        if (!has_space)
        {
            return false;
        }

        if (is_end_allocation) allocations.push_back({address, size});
        else allocations.insert(std::next(allocations.begin(), block_idx), {address, size});

        remaining_available_size -= size;
        return true;
    }

    void pool_allocation_tracker::deallocate(const u32 address)
    {
        ZoneScoped;
        u32 size = 0;
        const auto loc = std::ranges::find_if(allocations, [address, &size](const allocation_block& block)
        {
            if (block.start == address)
            {
                size = block.size;
                return true;
            }
            return false;
        });

        if (loc != allocations.end())
        {
            remaining_available_size += size;
            allocations.erase(loc);
        }
    }

    u32 pool_allocation_tracker::get_last_used_address() const
    {
        if (allocations.empty()) return 0;
        const allocation_block& last_block = allocations.back();
        return last_block.start + last_block.size;
    }

    void pool_allocation_tracker::resize(const u32 size)
    {
        remaining_available_size += size - total_pool_size;
        total_pool_size = size;
    }

    void pool_allocation_tracker::clear()
    {
        allocations.clear();
        remaining_available_size = total_pool_size;
    }

    bool pool_allocation_tracker::find_free(const u32 size, u32& start, u32& block_idx, bool& is_end_allocation) const
    {
        ZoneScoped;
        //Can't fit!
        if (size > remaining_available_size)
        {
            return false;
        }

        //Definitely fits
        if (allocations.empty())
        {
            start = 0;
            block_idx = 0;
            return true;
        }

        //Might fit at the end of the pool?
        const allocation_block& last_block = allocations.back();
        start = last_block.start + last_block.size;
        if (start + size <= total_pool_size)
        {
            block_idx = allocations.size();
            is_end_allocation = true;
            return true;
        }

        //Doesn't fit at the end - do we have space elsewhere?
        u32 remaining_possible_space = remaining_available_size - last_block.size;
        if (size > remaining_possible_space)
        {
            return false;
        }


        //Might have an empty space big enough from deallocations - iterate through and check
        //Performs best if elements are allocated and deallocated in roughly FIFO order
        start = 0;
        block_idx = 0;
        is_end_allocation = false;
        for (const allocation_block block : allocations)
        {
            if (block.start < start + size)
            {
                remaining_possible_space -= block.size;
                start = block.start + block.size;
                block_idx++;
            }
            else
            {
                return true;
            }

            if (size > remaining_possible_space)
            {
                return false;
            }
        }

        return false;
    }
}
