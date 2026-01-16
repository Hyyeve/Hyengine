#pragma once

#include "standard_data_buffer.hpp"
#include "../../common/pool_allocation_tracker.hpp"
#include "../../library/gl.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    using namespace hyengine;

    class pool_data_buffer
    {
    public:
        pool_data_buffer(const pool_data_buffer& other) = delete;                //COPY CONSTRUCTOR
        pool_data_buffer(pool_data_buffer&& other) = delete;                     //MOVE CONSTRUCTOR
        pool_data_buffer& operator=(const pool_data_buffer& other) = delete;     //COPY ASSIGNMENT
        pool_data_buffer& operator=(pool_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit pool_data_buffer();
        ~pool_data_buffer();

        [[nodiscard]] bool allocate(const GLsizeiptr size, const GLsizeiptr staging_size);
        void free();

        void shrink_staging_buffer();
        [[nodiscard]] bool reserve_staging_buffer_size(const u32 size);

        [[nodiscard]] bool try_allocate_space(const u32 size, u32& address);
        void deallocate_space(const u32 address);
        [[nodiscard]] u32 get_last_allocated_address() const;

        ///Blocks until the next slice of the staging buffer is available. Ensures all future operations cannot affect in-flight data.
        void next_staging_slice();

        ///Identical to next_staging_slice(), but with a timeout. If the function times out it will return false and does not gaurentee future operations are safe.
        [[nodiscard]] bool next_staging_slice_wait(const u64 timeout_nanos);

        [[nodiscard]] bool upload(const u32& address, const void* const data, const u32 size);

        void bind_state(const GLenum target) const;

        void bind_buffer_slot(GLenum target, u32 binding) const;
        void bind_buffer_range(GLenum target, u32 binding, GLintptr offset, GLsizeiptr size) const;

        [[nodiscard]] GLsizeiptr get_size() const;
        [[nodiscard]] GLuint get_buffer_id() const;

    private:
        [[nodiscard]] bool reallocate_staging_buffer(const GLsizeiptr size);

        standard_data_buffer pool_buffer;
        pool_allocation_tracker pool_allocator;

        standard_data_buffer staging_buffer;
        u32 current_staging_buffer_address;
        bool force_reallocate_staging_buffer = false;
    };
}
