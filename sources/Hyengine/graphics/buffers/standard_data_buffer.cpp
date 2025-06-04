#include "standard_data_buffer.hpp"
#include <thread>
#include "../../core/logger.hpp"

namespace hyengine::graphics {

    using namespace hyengine;

    standard_data_buffer::~standard_data_buffer()
    {
        free();
    }

    void standard_data_buffer::allocate_for_continual_writes(const GLenum target, const GLsizeiptr size)
    {
        constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        allocate(target, size, 3, nullptr, flags);
        map_storage(flags | GL_MAP_FLUSH_EXPLICIT_BIT);
    }

    void standard_data_buffer::allocate_for_staging_writes(const GLenum target, const GLsizeiptr size)
    {
        constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
        allocate(target, size, 3, nullptr, flags);
        map_storage(flags | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    }

    void standard_data_buffer::allocate_for_gpu_only(const GLenum target, const GLsizeiptr size)
    {
        allocate(target, size, 1, nullptr, 0);
    }

    void standard_data_buffer::allocate(const GLenum target, const GLsizeiptr size, const unsigned int slices, const void* const data, const GLbitfield storage_flags)
    {
        if (buffer_id != 0) {
            logger::error(logger_tag, "Couldn't allocate - already allocated with ID ", buffer_id);
            return;
        }

        if (slices < 1 || slices > 3)
        {
            logger::message_error("Couldn't allocate - slices must be between 1 and 3.", logger_tag);
            return;
        }

        total_size = size * slices;
        slice_count = slices;
        slice_size = size;
        buffer_target = target;

        unsigned int slice_offset = 0;
        for (slice_data& slice : buffer_slices)
        {
            slice.start_address = slice_offset;
            slice_offset += slice_size;
        }

        glCreateBuffers(1, &buffer_id);
        glNamedBufferStorage(buffer_id, total_size, data, storage_flags);

        logger::message_info(logger::format("Allocated data buffer ", buffer_id, ". ", logger::format_count(slice_count, "Slice"), ", Total ", logger::format_bytes(total_size)), logger_tag);
    }

    void standard_data_buffer::free()
    {
        unmap_storage();
        if (buffer_id == 0) return;

        logger::message_info(logger::format("Freeing data buffer ", buffer_id, ". ", logger::format_count(slice_count, "Slice"), " total ", logger::format_bytes(total_size)), logger_tag);

        glDeleteBuffers(1, &buffer_id);

        buffer_id = 0;
        current_slice_index = 1;
        buffer_target = 0;
    }

    void standard_data_buffer::map_storage(const GLbitfield mapping_flags)
    {
        if (mapped_pointer != nullptr) return;
        mapped_pointer = glMapNamedBufferRange(buffer_id, 0, total_size, mapping_flags);
    }

    void standard_data_buffer::unmap_storage()
    {
        if (mapped_pointer == nullptr) return;
        glUnmapNamedBuffer(buffer_id);
        mapped_pointer = nullptr;
    }

    void standard_data_buffer::flush_writes() const
    {
        if (mapped_pointer == nullptr) return;
        glFlushMappedNamedBufferRange(buffer_id, 0, total_size);
        glWaitSync(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0), 0, GL_TIMEOUT_IGNORED);
    }

    void standard_data_buffer::sync_fence()
    {
        GLsync& sync = buffer_slices[current_slice_index].fence;
        if (sync != nullptr) glDeleteSync(sync);
        sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        //glClientWaitSync(sync, 0, GL_TIMEOUT_IGNORED);
    }

    bool standard_data_buffer::sync_await(const unsigned long timeout_nanos) const
    {
        const GLsync& sync = buffer_slices[current_slice_index].fence;
        if (sync == nullptr) return true;
        const GLenum waitReturn = glClientWaitSync(sync, 0, timeout_nanos);
        if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED) return true;

        return false;
    }

    void standard_data_buffer::sync_blocking() const
    {
        while (!sync_await(GL_TIMEOUT_IGNORED))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void standard_data_buffer::increment_slice()
    {
        current_slice_index++;
        current_slice_index %= slice_count;
    }

    void standard_data_buffer::flush_and_fence()
    {
        flush_writes();
        sync_fence();
    }

    unsigned int standard_data_buffer::get_slice_offset() const
    {
        return buffer_slices[current_slice_index].start_address;
    }

    void* standard_data_buffer::get_mapped_pointer() const
    {
        return mapped_pointer;
    }

    void* standard_data_buffer::get_mapped_slice_pointer() const
    {
        if (mapped_pointer == nullptr) return nullptr;
        return static_cast<GLbyte*>(mapped_pointer) + get_slice_offset();
    }

    GLuint standard_data_buffer::get_buffer_id() const
    {
        return buffer_id;
    }

    GLenum standard_data_buffer::get_target() const
    {
        return buffer_target;
    }

    GLsizeiptr standard_data_buffer::get_size() const
    {
        return total_size;
    }

    GLsizeiptr standard_data_buffer::get_slice_size() const
    {
        return slice_size;
    }

    void standard_data_buffer::bind_state() const
    {
        glBindBuffer(buffer_target, buffer_id);
    }

    void standard_data_buffer::unbind_state() const
    {
        glBindBuffer(buffer_target, 0);
    }

    void standard_data_buffer::bind_slice_base(const int binding) const
    {
        bind_slice_range(binding, 0, slice_size);
    }

    void standard_data_buffer::bind_slice_range(const int binding, const GLintptr offset, const GLsizeiptr bytes) const
    {
        bind_buffer_range(binding, get_slice_offset() + offset, bytes);
    }

    void standard_data_buffer::bind_buffer_base(const int binding) const
    {
        glBindBufferBase(buffer_target, binding, buffer_id);
    }

    void standard_data_buffer::bind_buffer_range(const int binding, const GLintptr offset, const GLsizeiptr bytes) const
    {
        if (offset + bytes > total_size)
        {
            logger::message_warn(logger::format("Can't bind range - requested start and size would overflow", " (buffer ", buffer_id, ")"), logger_tag);
            return;
        }

        glBindBufferRange(buffer_target, binding, buffer_id, offset, bytes);
    }

    void standard_data_buffer::copy_slice_data(const GLuint source_buffer_id) const
    {
        copy_slice_range(source_buffer_id, 0, 0, slice_size);
    }

    void standard_data_buffer::copy_slice_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr bytes) const
    {
        copy_buffer_range(source_buffer_id, read_offset, get_slice_offset() + write_offset, bytes);
    }

    void standard_data_buffer::copy_buffer_data(const GLuint source_buffer_id) const
    {
        copy_buffer_range(source_buffer_id, 0, 0, total_size);
    }

    void standard_data_buffer::copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr bytes) const
    {
        if (write_offset + bytes > total_size)
        {
            logger::message_warn(logger::format("Can't copy range - requested start and size would overflow", " (buffer ", buffer_id, ")"), logger_tag);
            return;
        }

        glCopyNamedBufferSubData(source_buffer_id, buffer_id, read_offset, write_offset, bytes);
    }
}
