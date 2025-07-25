#pragma once

#include "pool_data_buffer.hpp"

namespace hyengine
{
    template <typename TYPE>
    class typed_pool_buffer
    {
    public:
        typed_pool_buffer(const typed_pool_buffer& other) = delete; //COPY CONSTRUCTOR
        typed_pool_buffer(typed_pool_buffer&& other) = delete; //MOVE CONSTRUCTOR
        typed_pool_buffer& operator=(const typed_pool_buffer& other) = delete; //COPY ASSIGNMENT
        typed_pool_buffer& operator=(typed_pool_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit typed_pool_buffer() = default;

        ~typed_pool_buffer()
        {
            free();
        }

        void allocate(const GLenum target, const GLsizeiptr elements)
        {
            internal_data_buffer.allocate(target, elements * sizeof(TYPE));
        }

        void free()
        {
            internal_data_buffer.free();
        }

        void shrink_staging_buffer()
        {
            internal_data_buffer.shrink_staging_buffer();
        }

        bool try_allocate_space(const unsigned int elements, unsigned int& address)
        {
            return internal_data_buffer.try_allocate_space(elements * sizeof(TYPE), address);
        }

        void deallocate_space(const unsigned int address)
        {
            internal_data_buffer.deallocate_space(address);
        }

        [[nodiscard]] unsigned int get_last_allocated_element() const
        {
            return internal_data_buffer.get_last_allocated_address() / sizeof(TYPE);
        }

        void queue_upload(const unsigned int& address, const TYPE* const data, const unsigned int elements)
        {
            internal_data_buffer.queue_upload(address, data, elements * sizeof(TYPE));
        }

        void submit_uploads()
        {
            internal_data_buffer.submit_uploads();
        }

        void bind_state() const
        {
            internal_data_buffer.bind_state();
        }

        void unbind_state() const
        {
            internal_data_buffer.unbind_state();
        }

        void bind_buffer_base(const int binding) const
        {
            internal_data_buffer.bind_buffer_base(binding);
        }

        void bind_buffer_range(const int binding, const GLintptr index, const GLsizeiptr elements) const
        {
            internal_data_buffer.bind_buffer_range(binding, index * sizeof(TYPE), elements * sizeof(TYPE));
        }

        [[nodiscard]] GLsizeiptr get_elements() const
        {
            return internal_data_buffer.get_size() / sizeof(TYPE);
        }

        [[nodiscard]] GLuint get_buffer_id() const
        {
            return internal_data_buffer.get_buffer_id();
        }

        [[nodiscard]] GLenum get_target() const
        {
            return internal_data_buffer.get_target();
        }

        [[nodiscard]] unsigned int get_pending_upload_count() const
        {
            return internal_data_buffer.get_pending_upload_count();
        }

    private:
        pool_data_buffer internal_data_buffer;
    };
}
