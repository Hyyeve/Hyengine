#pragma once
#include <array>
#include <stdexcept>
#include "../../library/gl.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    /* MULTIBUFFERING ORDER OF OPERATIONS

    Increment to next slice
    Sync await / blocking until slice is ready
    Write data into slice
    Flush & fence - inserts a gl-side sync point for writes to be flushed
    Draw slice - as many times as wanted (using bind_state and get_slice_offset())
    Repeat

    */

    class standard_data_buffer {
    public:
        standard_data_buffer(const standard_data_buffer& other) = delete; //COPY CONSTRUCTOR
        standard_data_buffer(standard_data_buffer&& other) = delete; //MOVE CONSTRUCTOR
        standard_data_buffer& operator=(const standard_data_buffer& other) = delete; //COPY ASSIGNMENT
        standard_data_buffer& operator=(standard_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit standard_data_buffer() = default;
        ~standard_data_buffer();

        void allocate_for_continual_writes(const GLenum target, const GLsizeiptr size);
        void allocate_for_staging_writes(const GLenum target, const GLsizeiptr size);
        void allocate_for_gpu_only(const GLenum target, const GLsizeiptr size);

        void allocate(const GLenum target, const GLsizeiptr size, const unsigned int slices, const void* const data, const GLbitfield storage_flags);
        void free();

        void map_storage(const GLbitfield mapping_flags);
        void unmap_storage();
        void flush_writes() const;

        void bind_state() const;
        void unbind_state() const;

        void bind_buffer_base(const int binding) const;
        void bind_buffer_range(const int binding, const GLintptr offset, const GLsizeiptr bytes) const;

        void copy_buffer_data(const GLuint source_buffer_id) const;
        void copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const;

        void bind_slice_base(const int binding) const;
        void bind_slice_range(const int binding, const GLintptr offset, const GLsizeiptr bytes) const;

        void copy_slice_data(const GLuint source_buffer_id) const;
        void copy_slice_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const;

        void increment_slice();

        void flush_and_fence();
        void sync_fence();
        [[nodiscard]] bool sync_await(const unsigned long timeout_nanos) const;
        void sync_blocking() const;

        [[nodiscard]] unsigned int get_slice_offset() const;

        [[nodiscard]] void* get_mapped_pointer() const;
        [[nodiscard]] void* get_mapped_slice_pointer() const;

        [[nodiscard]] GLuint get_buffer_id() const;
        [[nodiscard]] GLenum get_target() const;
        [[nodiscard]] GLsizeiptr get_size() const;
        [[nodiscard]] GLsizeiptr get_slice_size() const;

        template <typename TYPE>
        TYPE& data(unsigned int index)
        {
            return static_cast<TYPE*>(get_mapped_slice_pointer())[index];
        }

    private:
        const std::string logger_tag = "GPU Slice Buffer";

        struct slice_data {
            GLintptr start_address = 0;
            GLsync fence = nullptr;
        };


        unsigned int current_slice_index = 0;
        void* mapped_pointer = nullptr;

        //Set on allocation only
        GLenum buffer_target = 0;
        GLuint buffer_id = 0;
        GLsizeiptr total_size = 0;
        GLsizeiptr slice_size = 0;
        unsigned int slice_count = 1;
        std::array<slice_data, 3> buffer_slices;
    };
}
