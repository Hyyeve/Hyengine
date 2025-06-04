#pragma once

#include "standard_data_buffer.hpp"

namespace hyengine::graphics {

    using namespace hyengine;


    template<typename TYPE>
    class typed_data_buffer
    {
    public:
        typed_data_buffer(const typed_data_buffer& other) = delete; //COPY CONSTRUCTOR
        typed_data_buffer(typed_data_buffer&& other) = delete; //MOVE CONSTRUCTOR
        typed_data_buffer& operator=(const typed_data_buffer& other) = delete; //COPY ASSIGNMENT
        typed_data_buffer& operator=(typed_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT


        explicit typed_data_buffer() = default;

        void allocate_for_continual_writes(const GLenum target, const GLsizeiptr elements) { internal_data_buffer.allocate_for_continual_writes(target, elements * sizeof(TYPE)); }
        void allocate_for_staging_writes(const GLenum target, const GLsizeiptr elements) { internal_data_buffer.allocate_for_staging_writes(target, elements * sizeof(TYPE)); }
        void allocate_for_gpu_only(const GLenum target, const GLsizeiptr elements) { internal_data_buffer.allocate_for_gpu_only(target, elements * sizeof(TYPE)); }

        void allocate(const GLenum target, const GLsizeiptr elements, const unsigned int slices, const TYPE* const data, const GLbitfield storage_flags)
        { internal_data_buffer.allocate(target, elements * sizeof(TYPE), slices, data, storage_flags); }

        void free() { internal_data_buffer.free(); }

        void map_storage(const GLbitfield mapping_flags) { internal_data_buffer.map_storage(mapping_flags); }
        void unmap_storage() { internal_data_buffer.unmap_storage(); }
        void flush_writes() const { internal_data_buffer.flush_writes(); }

        void bind_state() const { internal_data_buffer.bind_state(); }
        void unbind_state() const { internal_data_buffer.unbind_state(); }

        void bind_buffer_base(const int binding) const { internal_data_buffer.bind_buffer_base(binding); }
        void bind_buffer_range(const int binding, const GLintptr index, const GLsizeiptr elements) const
        { internal_data_buffer.bind_buffer_range(binding, index * sizeof(TYPE), elements * sizeof(TYPE)); }

        void copy_buffer_data(const GLuint source_buffer_id) const { internal_data_buffer.copy_buffer_data(source_buffer_id); }
        void copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_index, const GLintptr write_index, const GLintptr elements) const
        { internal_data_buffer.copy_buffer_range(source_buffer_id, read_index * sizeof(TYPE), write_index * sizeof(TYPE), elements * sizeof(TYPE)); }

        void bind_slice_base(const int binding) const { internal_data_buffer.bind_slice_base(binding); }
        void bind_slice_range(const int binding, const GLintptr index, const GLsizeiptr elements) const
        { internal_data_buffer.bind_slice_range(binding, index * sizeof(TYPE), elements * sizeof(TYPE));}

        void copy_slice_data(const GLuint source_buffer_id) const { internal_data_buffer.copy_slice_data(source_buffer_id); }
        void copy_slice_range(const GLuint source_buffer_id, const GLintptr read_index, const GLintptr write_index, const GLintptr elements) const
        { internal_data_buffer.copy_slice_range(source_buffer_id, read_index * sizeof(TYPE), write_index * sizeof(TYPE), elements * sizeof(TYPE)); }

        void increment_slice() { internal_data_buffer.increment_slice();}

        void flush_and_fence() { internal_data_buffer.flush_and_fence(); }

        void sync_fence() { internal_data_buffer.sync_fence(); }
        [[nodiscard]] bool sync_await(const unsigned long timeout_nanos) const { return internal_data_buffer.sync_await(timeout_nanos); }
        void sync_blocking() const { return internal_data_buffer.sync_blocking(); }

        [[nodiscard]] unsigned int get_slice_first_element() const
        { return internal_data_buffer.get_slice_offset() / sizeof(TYPE); }
        [[nodiscard]] unsigned int get_slice_offset() const
        { return internal_data_buffer.get_slice_offset(); }
        [[nodiscard]] TYPE* get_mapped_pointer() const
        { return static_cast<TYPE*>(internal_data_buffer.get_mapped_pointer()); }
        [[nodiscard]] TYPE* get_mapped_slice_pointer() const
        { return static_cast<TYPE*>(internal_data_buffer.get_mapped_slice_pointer());}

        [[nodiscard]] GLuint get_buffer_id() const { return internal_data_buffer.get_buffer_id(); }
        [[nodiscard]] GLenum get_target() const { return internal_data_buffer.get_target(); }
        [[nodiscard]] GLsizeiptr get_max_elements() const { return internal_data_buffer.get_size() / sizeof(TYPE); }
        [[nodiscard]] GLsizeiptr get_max_slice_elements() const { return internal_data_buffer.get_slice_size() / sizeof(TYPE); }
        [[nodiscard]] static GLsizei get_element_size() { return sizeof(TYPE); }

        TYPE& data(const unsigned int index)
        {
            return internal_data_buffer.data<TYPE>(index);
        }

    private:
        standard_data_buffer internal_data_buffer;
    };

}
