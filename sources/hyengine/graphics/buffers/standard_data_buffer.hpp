#pragma once
#include <array>
#include <stdexcept>
#include "../../library/gl.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    /* MULTIBUFFERING ORDER OF OPERATIONS


    */

    class standard_data_buffer
    {
    public:
        standard_data_buffer(const standard_data_buffer& other) = delete;                //COPY CONSTRUCTOR
        standard_data_buffer(standard_data_buffer&& other) = delete;                     //MOVE CONSTRUCTOR
        standard_data_buffer& operator=(const standard_data_buffer& other) = delete;     //COPY ASSIGNMENT
        standard_data_buffer& operator=(standard_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit standard_data_buffer() = default;
        ~standard_data_buffer();

        [[nodiscard]] bool allocate_for_cpu_writes(const GLsizeiptr size);
        [[nodiscard]] bool allocate_for_gpu_writes(const GLsizeiptr size);

        [[nodiscard]] bool allocate(const GLsizeiptr size, const u32 slices, const void* const data, const GLbitfield storage_flags);
        void free();

        void map_storage(const GLbitfield mapping_flags);
        void unmap_storage();

        void bind_state(const GLenum target) const;

        ///Bind the buffer to a special binding index (shader storage, uniform block, etc)
        void bind_buffer_slot(const GLenum target, const u32 binding) const;

        ///Bind a range of the buffer to a special binding index (shader storage, uniform block, etc)
        void bind_buffer_range(const GLenum target, const u32 binding, const GLintptr offset, const GLsizeiptr bytes) const;

        void copy_buffer_data(const GLuint source_buffer_id) const;
        void copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const;

        ///Bind the current slice to a special binding index (shader storage, uniform block, etc)
        void bind_slice_slot(const GLenum target, const u32 binding) const;

        ///Bind a range of the current slice to a special binding index (shader storage, uniform block, etc)
        void bind_slice_range(const GLenum target, const u32 binding, const GLintptr offset, const GLsizeiptr bytes) const;

        void copy_slice_data(const GLuint source_buffer_id) const;
        void copy_slice_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const;

        void upload(const u32& address, const void* const data, const u32 size) const;

        ///Blocks until able to move to the next slice. Ensures all future operations cannot affect in-flight data.
        void next_slice();

        ///Identical to next_slice(), but with a timeout. If the function times out it will return false and the buffer slice will not have advanced.
        [[nodiscard]] bool next_slice_wait(const u64 timeout_nanos);

        [[nodiscard]] u32 get_slice_offset() const;

        [[nodiscard]] void* get_mapped_pointer() const;
        [[nodiscard]] void* get_mapped_slice_pointer() const;

        [[nodiscard]] GLuint get_buffer_id() const;
        [[nodiscard]] GLsizeiptr get_size() const;
        [[nodiscard]] GLsizeiptr get_slice_size() const;

        template <typename type>
        type& data(u32 index)
        {
            return static_cast<type*>(get_mapped_slice_pointer())[index];
        }

    private:
        struct slice_data
        {
            GLintptr start_address = 0;
            GLsync fence = nullptr;
        };

        void increment_slice();
        void decrement_slice();

        void sync_fence();
        [[nodiscard]] bool sync_await(const u64 timeout_nanos) const;
        void sync_block() const;

        u32 current_slice_index = 0;
        void* mapped_pointer = nullptr;

        GLuint buffer_id = 0;
        GLsizeiptr total_size = 0;
        GLsizeiptr slice_size = 0;
        u32 slice_count = 1;
        std::array<slice_data, 3> buffer_slices;
    };
}
