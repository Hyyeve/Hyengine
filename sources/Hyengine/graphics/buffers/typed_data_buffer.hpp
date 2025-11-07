#pragma once

#include "standard_data_buffer.hpp"

namespace hyengine
{
    template <typename TYPE>
    class typed_data_buffer
    {
    public:
        typed_data_buffer(const typed_data_buffer& other) = delete; //COPY CONSTRUCTOR
        typed_data_buffer(typed_data_buffer&& other) = delete; //MOVE CONSTRUCTOR
        typed_data_buffer& operator=(const typed_data_buffer& other) = delete; //COPY ASSIGNMENT
        typed_data_buffer& operator=(typed_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT


        explicit typed_data_buffer() = default;

        void allocate_for_cpu_writes(const GLenum target, const GLsizeiptr elements)
        {
            internal_data_buffer.allocate_for_cpu_writes(target, elements * sizeof(TYPE));
        }

        void allocate_for_gpu_writes(const GLenum target, const GLsizeiptr elements)
        {
            internal_data_buffer.allocate_for_gpu_writes(target, elements * sizeof(TYPE));
        }

        void allocate(const GLenum target, const GLsizeiptr elements, const u32 slices, const TYPE* const data, const GLbitfield storage_flags)
        {
            internal_data_buffer.allocate(target, elements * sizeof(TYPE), slices, data, storage_flags);
        }

        void free()
        {
            internal_data_buffer.free();
        }

        void map_storage(const GLbitfield mapping_flags)
        {
            internal_data_buffer.map_storage(mapping_flags);
        }

        void unmap_storage()
        {
            internal_data_buffer.unmap_storage();
        }

        void bind_state() const
        {
            internal_data_buffer.bind_state();
        }

        void unbind_state() const
        {
            internal_data_buffer.unbind_state();
        }

        void bind_buffer_base(const GLenum target, const i32 binding) const
        {
            internal_data_buffer.bind_buffer_base(target, binding);
        }

        void bind_buffer_range(const GLenum target, const i32 binding, const GLintptr index, const GLsizeiptr elements) const
        {
            internal_data_buffer.bind_buffer_range(target, binding, index * sizeof(TYPE), elements * sizeof(TYPE));
        }

        void copy_buffer_data(const GLuint source_buffer_id) const
        {
            internal_data_buffer.copy_buffer_data(source_buffer_id);
        }

        void copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_index, const GLintptr write_index, const GLintptr elements) const
        {
            internal_data_buffer.copy_buffer_range(source_buffer_id, read_index * sizeof(TYPE), write_index * sizeof(TYPE), elements * sizeof(TYPE));
        }

        void bind_slice_base(const GLenum target, const i32 binding) const
        {
            internal_data_buffer.bind_slice_base(target, binding);
        }

        void bind_slice_range(const GLenum target, const i32 binding, const GLintptr index, const GLsizeiptr elements) const
        {
            internal_data_buffer.bind_slice_range(target, binding, index * sizeof(TYPE), elements * sizeof(TYPE));
        }

        void copy_slice_data(const GLuint source_buffer_id) const
        {
            internal_data_buffer.copy_slice_data(source_buffer_id);
        }

        void copy_slice_range(const GLuint source_buffer_id, const GLintptr read_index, const GLintptr write_index, const GLintptr elements) const
        {
            internal_data_buffer.copy_slice_range(source_buffer_id, read_index * sizeof(TYPE), write_index * sizeof(TYPE), elements * sizeof(TYPE));
        }

        void block_ready()
        {
            internal_data_buffer.block_ready();
        }

        void upload(const u32& address, const TYPE* data, const u32 elements)
        {
            internal_data_buffer.upload(address, data, elements * sizeof(TYPE));
        }

        [[nodiscard]] bool await_ready(const unsigned long timeout_nanos)
        {
            return internal_data_buffer.await_ready(timeout_nanos);
        }

        [[nodiscard]] u32 get_slice_first_element() const
        {
            return internal_data_buffer.get_slice_offset() / sizeof(TYPE);
        }

        [[nodiscard]] u32 get_slice_offset() const
        {
            return internal_data_buffer.get_slice_offset();
        }

        [[nodiscard]] TYPE* get_mapped_pointer() const
        {
            return static_cast<TYPE*>(internal_data_buffer.get_mapped_pointer());
        }

        [[nodiscard]] TYPE* get_mapped_slice_pointer() const
        {
            return static_cast<TYPE*>(internal_data_buffer.get_mapped_slice_pointer());
        }

        [[nodiscard]] GLuint get_buffer_id() const
        {
            return internal_data_buffer.get_buffer_id();
        }

        [[nodiscard]] GLenum get_target() const
        {
            return internal_data_buffer.get_target();
        }

        [[nodiscard]] GLsizeiptr get_max_elements() const
        {
            return internal_data_buffer.get_size() / sizeof(TYPE);
        }

        [[nodiscard]] GLsizeiptr get_max_slice_elements() const
        {
            return internal_data_buffer.get_slice_size() / sizeof(TYPE);
        }

        //It makes a lot more sense on the calling side to treat this as an instance function
        // ReSharper disable once CppMemberFunctionMayBeStatic
        [[nodiscard]] GLsizei get_element_size()
        {
            return sizeof(TYPE);
        }

        TYPE& data(const u32 index)
        {
            return internal_data_buffer.data<TYPE>(index);
        }

    private:
        standard_data_buffer internal_data_buffer;
    };
}
