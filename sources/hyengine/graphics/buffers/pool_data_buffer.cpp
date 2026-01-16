#include "pool_data_buffer.hpp"
#include <tracy/Tracy.hpp>
#include "../../core/logger.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace hyengine
{
    using namespace hyengine;

    pool_data_buffer::pool_data_buffer() : pool_allocator(0), current_staging_buffer_address(0) {}

    pool_data_buffer::~pool_data_buffer()
    {
        free();
    }

    bool pool_data_buffer::allocate(const GLsizeiptr size, const GLsizeiptr staging_size)
    {
        ZoneScoped;
        TracyGpuZone("allocate pool staging buffer");
        const bool pool_buffer_allocated = pool_buffer.allocate_for_gpu_writes(size);
        pool_allocator = pool_allocation_tracker(size);
        const bool staging_buffer_allocated = reallocate_staging_buffer(staging_size);

        if (!pool_buffer_allocated || !staging_buffer_allocated)
        {
            log_error(logger_tags::GRAPHICS, "Pool data buffer allocation failed.");
            free();
            return false;
        }

        log_debug(logger_tags::GRAPHICS, "Buffer ", pool_buffer.get_buffer_id(), " allocated as pool buffer.");
        return true;
    }

    void pool_data_buffer::free()
    {
        ZoneScoped;
        TracyGpuZone("free pool staging buffer");
        pool_buffer.free();
        staging_buffer.free();
    }

    void pool_data_buffer::shrink_staging_buffer()
    {
        force_reallocate_staging_buffer = true;
    }

    bool pool_data_buffer::reserve_staging_buffer_size(const u32 size)
    {
        if (staging_buffer.get_slice_size() < size)
        {
            return reallocate_staging_buffer(size);
        }

        return true;
    }

    bool pool_data_buffer::try_allocate_space(const u32 size, u32& address)
    {
        return pool_allocator.try_allocate(size, address);
    }

    void pool_data_buffer::deallocate_space(const u32 address)
    {
        pool_allocator.deallocate(address);
    }

    u32 pool_data_buffer::get_last_allocated_address() const
    {
        return pool_allocator.get_last_used_address();
    }

    void pool_data_buffer::next_staging_slice()
    {
        ZoneScoped;
        staging_buffer.next_slice();
        current_staging_buffer_address = 0;
    }

    bool pool_data_buffer::next_staging_slice_wait(const u64 timeout_nanos)
    {
        ZoneScoped;
        const bool success = staging_buffer.next_slice_wait(timeout_nanos);
        if (success) current_staging_buffer_address = 0;
        return success;
    }

    bool pool_data_buffer::upload(const u32& address, const void* const data, const u32 size)
    {
        ZoneScoped;
        const u32 upload_offset = current_staging_buffer_address;
        current_staging_buffer_address += size;

        const GLsizeiptr staging_buffer_size = staging_buffer.get_slice_size();
        if (current_staging_buffer_address > staging_buffer_size || force_reallocate_staging_buffer)
        {
            const bool could_reallocate = reallocate_staging_buffer(current_staging_buffer_address * 2);
            if (!could_reallocate)
            {
                log_error(logger_tags::GRAPHICS, "Pool data buffer upload failed! (buffer ", get_buffer_id(), ")");
                current_staging_buffer_address -= size;
                return false;
            }
            current_staging_buffer_address = 0;
        }

        GLbyte* upload_buffer_pointer = static_cast<GLbyte*>(staging_buffer.get_mapped_slice_pointer());
        const GLbyte* source_pointer = static_cast<const GLbyte*>(data);
        memcpy(upload_buffer_pointer + upload_offset, source_pointer, size);

        pool_buffer.copy_buffer_range(staging_buffer.get_buffer_id(), staging_buffer.get_slice_offset() + upload_offset, address, size);
        return true;
    }

    void pool_data_buffer::bind_state(const GLenum target) const
    {
        pool_buffer.bind_state(target);
    }

    void pool_data_buffer::bind_buffer_slot(const GLenum target, const u32 binding) const
    {
        pool_buffer.bind_buffer_slot(target, binding);
    }

    void pool_data_buffer::bind_buffer_range(const GLenum target, const u32 binding, const GLintptr offset, const GLsizeiptr size) const
    {
        pool_buffer.bind_buffer_range(target, binding, offset, size);
    }

    GLsizeiptr pool_data_buffer::get_size() const
    {
        return pool_buffer.get_size();
    }

    GLuint pool_data_buffer::get_buffer_id() const
    {
        return pool_buffer.get_buffer_id();
    }

    bool pool_data_buffer::reallocate_staging_buffer(const GLsizeiptr size)
    {
        ZoneScoped;
        TracyGpuZone("pool staging buffer reallocate");
        log_debug(logger_tags::GRAPHICS, "Allocating pool staging buffer to ", stringify_bytes(size), ".");
        staging_buffer.free();
        const bool could_allocate = staging_buffer.allocate_for_cpu_writes(size);
        force_reallocate_staging_buffer = false;

        if (!could_allocate)
        {
            log_error(logger_tags::GRAPHICS, "Failed to reallocate pool staging buffer!");
            return false;
        }

        log_debug(logger_tags::GRAPHICS, "Buffer ", staging_buffer.get_buffer_id(), " allocated as pool staging buffer.");
        return true;
    }
}
