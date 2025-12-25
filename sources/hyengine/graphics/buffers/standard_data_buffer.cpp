#include "standard_data_buffer.hpp"
#include <thread>
#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"
#include "hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    standard_data_buffer::~standard_data_buffer()
    {
        free();
    }

    bool standard_data_buffer::allocate_for_cpu_writes(const GLenum target, const GLsizeiptr size)
    {
        constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        const bool success = allocate(target, size, 3, nullptr, flags);
        if (!success) return false;
        map_storage(flags);
        return true;
    }

    bool standard_data_buffer::allocate_for_gpu_writes(const GLenum target, const GLsizeiptr size)
    {
        return allocate(target, size, 1, nullptr, 0);
    }

    bool standard_data_buffer::allocate(const GLenum target, const GLsizeiptr size, const u32 slices, const void* const data, const GLbitfield storage_flags)
    {
        ZoneScoped;
        if (buffer_id != 0)
        {
            log_error(logger_tags::GRAPHICS, "Couldn't allocate - already allocated with ID ", buffer_id);
            return false;
        }

        if (slices < 1 || slices > 3)
        {
            log_error(logger_tags::GRAPHICS, "Couldn't allocate - slices must be between 1 and 3.");
            return false;
        }

        total_size = size * slices;
        slice_count = slices;
        slice_size = size;
        buffer_target = target;

        u32 slice_offset = 0;
        for (slice_data& slice : buffer_slices)
        {
            slice.start_address = slice_offset;
            slice_offset += slice_size;
        }

        glCreateBuffers(1, &buffer_id);

        if (buffer_id == 0)
        {
            log_error(logger_tags::GRAPHICS, "Failed to allocate data buffer (buffer ", buffer_id, ")");
            return false;
        }

        glNamedBufferStorage(buffer_id, total_size, data, storage_flags);
        log_debug(logger_tags::GRAPHICS, "Allocated data buffer ", buffer_id, ". ", stringify_count(slice_count, "Slice"), ", Total ", stringify_bytes(total_size));
        return true;
    }

    void standard_data_buffer::free()
    {
        ZoneScoped;
        unmap_storage();
        if (buffer_id == 0) return;

        log_debug(logger_tags::GRAPHICS, "Freeing data buffer ", buffer_id, ". ", stringify_count(slice_count, "Slice"), " total ", stringify_bytes(total_size));

        glDeleteBuffers(1, &buffer_id);

        buffer_id = 0;
        current_slice_index = 1;
        buffer_target = 0;
        total_size = 0;
        slice_size = 0;
        slice_count = 1;

        for (slice_data& slice : buffer_slices)
        {
            slice.start_address = 0;
            if (slice.fence != nullptr)
                glDeleteSync(slice.fence);
            slice.fence = nullptr;
        }
    }

    void standard_data_buffer::map_storage(const GLbitfield mapping_flags)
    {
        ZoneScoped;
        if (mapped_pointer != nullptr) return;
        mapped_pointer = glMapNamedBufferRange(buffer_id, 0, total_size, mapping_flags);
    }

    void standard_data_buffer::unmap_storage()
    {
        ZoneScoped;
        if (mapped_pointer == nullptr) return;
        glUnmapNamedBuffer(buffer_id);
        mapped_pointer = nullptr;
    }

    void standard_data_buffer::bind_state() const
    {
        ZoneScoped;
        glBindBuffer(buffer_target, buffer_id);
    }

    void standard_data_buffer::unbind_state() const
    {
        ZoneScoped;
        glBindBuffer(buffer_target, 0);
    }

    void standard_data_buffer::bind_buffer_base(const GLenum target, const i32 binding) const
    {
        ZoneScoped;
        glBindBufferBase(target, binding, buffer_id);
    }

    void standard_data_buffer::bind_buffer_range(const GLenum target, const i32 binding, const GLintptr offset, const GLsizeiptr bytes) const
    {
        ZoneScoped;
        if (offset + bytes > total_size)
        {
            log_warn(logger_tags::GRAPHICS, "Can't bind range - requested start and size would overflow", " (buffer ", buffer_id, ")");
            return;
        }

        glBindBufferRange(target, binding, buffer_id, offset, bytes);
    }

    void standard_data_buffer::copy_buffer_data(const GLuint source_buffer_id) const
    {
        copy_buffer_range(source_buffer_id, 0, 0, total_size);
    }

    void standard_data_buffer::copy_buffer_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr bytes) const
    {
        ZoneScoped;
        if (write_offset + bytes > total_size)
        {
            log_warn(logger_tags::GRAPHICS, "Can't copy range - requested start and size would overflow", " (buffer ", buffer_id, ")");
            return;
        }

        glCopyNamedBufferSubData(source_buffer_id, buffer_id, read_offset, write_offset, bytes);
    }

    void standard_data_buffer::bind_slice_base(const GLenum target, const i32 binding) const
    {
        bind_slice_range(target, binding, 0, slice_size);
    }

    void standard_data_buffer::bind_slice_range(const GLenum target, const i32 binding, const GLintptr offset, const GLsizeiptr bytes) const
    {
        bind_buffer_range(target, binding, get_slice_offset() + offset, bytes);
    }

    void standard_data_buffer::copy_slice_data(const GLuint source_buffer_id) const
    {
        copy_slice_range(source_buffer_id, 0, 0, slice_size);
    }

    void standard_data_buffer::copy_slice_range(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLsizeiptr bytes) const
    {
        copy_buffer_range(source_buffer_id, read_offset, get_slice_offset() + write_offset, bytes);
    }

    void standard_data_buffer::block_ready()
    {
        ZoneScoped;
        sync_fence();      //Set sync point for everything done with previous buffer slice data
        increment_slice(); //Move to next buffer slice
        sync_block();      //Wait for buffer slice to finish being used
    }

    void standard_data_buffer::upload(const u32& address, const void* const data, const u32 size) const
    {
        ZoneScoped;
        if (mapped_pointer == nullptr)
        {
            log_error(logger_tags::GRAPHICS, "Buffer ", buffer_id, ", cannot be uploaded to directly! Not currently mapped.");
            return;
        }

        GLbyte* dest_pointer = static_cast<GLbyte*>(get_mapped_slice_pointer());
        const GLbyte* source_pointer = static_cast<const GLbyte*>(data);
        memcpy(dest_pointer + address, source_pointer, size);
    }

    bool standard_data_buffer::await_ready(const u64 timeout_nanos)
    {
        ZoneScoped;
        sync_fence();
        increment_slice();
        const bool result = sync_await(timeout_nanos);
        if (!result) decrement_slice(); //A failed await should leave us on the previous slice
        return result;
    }

    u32 standard_data_buffer::get_slice_offset() const
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

    void standard_data_buffer::increment_slice()
    {
        current_slice_index = (current_slice_index + 1) % slice_count;
    }

    void standard_data_buffer::decrement_slice()
    {
        current_slice_index = (static_cast<long>(current_slice_index) - 1) % slice_count;
    }

    void standard_data_buffer::sync_fence()
    {
        ZoneScoped;
        GLsync& sync = buffer_slices[current_slice_index].fence;
        if (sync != nullptr)
            glDeleteSync(sync);
        sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    bool standard_data_buffer::sync_await(const u64 timeout_nanos) const
    {
        ZoneScoped;
        const GLsync& sync = buffer_slices[current_slice_index].fence;
        if (sync == nullptr) return true;
        const GLenum wait_return = glClientWaitSync(sync, 0, timeout_nanos);
        if (wait_return == GL_ALREADY_SIGNALED || wait_return == GL_CONDITION_SATISFIED) return true;

        return false;
    }

    void standard_data_buffer::sync_block() const
    {
        ZoneScoped;
        while (!sync_await(GL_TIMEOUT_IGNORED))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
