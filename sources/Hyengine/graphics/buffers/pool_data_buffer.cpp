#include "pool_data_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"


namespace hyengine
{
    using namespace hyengine;

    pool_data_buffer::pool_data_buffer(): pool_allocator(0)
    {
    }

    pool_data_buffer::~pool_data_buffer()
    {
        free();
    }

    void pool_data_buffer::allocate(const GLenum target, const GLsizeiptr size, const GLsizeiptr staging_size)
    {
        ZoneScoped;
        pool_buffer.allocate_for_gpu_writes(target, size);
        pool_allocator = pool_allocation_tracker(size);
        reallocate_staging_buffer(staging_size);
        log_info(logger_tag, "Buffer ", pool_buffer.get_buffer_id(), " allocated as pool buffer.");
    }

    void pool_data_buffer::free()
    {
        ZoneScoped;
        pool_buffer.free();
        staging_buffer.free();
    }

    void pool_data_buffer::shrink_staging_buffer()
    {
        force_reallocate_staging_buffer = true;
    }

    void pool_data_buffer::reserve_staging_buffer_size(const u32 size)
    {
        if (staging_buffer.get_slice_size() < size)
        {
            reallocate_staging_buffer(size);
        }
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

    void pool_data_buffer::queue_upload(const u32& address, const void* const data, const u32 size)
    {
        pending_uploads.push_back({data, 0, address, size});
    }

    void pool_data_buffer::bind_state() const
    {
        pool_buffer.bind_state();
    }

    void pool_data_buffer::unbind_state() const
    {
        pool_buffer.unbind_state();
    }

    void pool_data_buffer::bind_buffer_base(const i32 binding) const
    {
        pool_buffer.bind_buffer_base(binding);
    }

    void pool_data_buffer::bind_buffer_range(const i32 binding, const GLintptr offset, const GLsizeiptr size) const
    {
        pool_buffer.bind_buffer_range(binding, offset, size);
    }

    GLsizeiptr pool_data_buffer::get_size() const
    {
        return pool_buffer.get_size();
    }

    GLuint pool_data_buffer::get_buffer_id() const
    {
        return pool_buffer.get_buffer_id();
    }

    GLenum pool_data_buffer::get_target() const
    {
        return pool_buffer.get_target();
    }

    u32 pool_data_buffer::get_pending_upload_count() const
    {
        return pending_uploads.size();
    }

    void pool_data_buffer::submit_uploads()
    {
        ZoneScoped;
        if (pending_uploads.empty())
        {
            return;
        }

        staging_buffer.block_ready();

        u32 staging_buffer_location = 0;
        for (upload_info& upload : pending_uploads)
        {
            upload.upload_buffer_address = staging_buffer_location;
            staging_buffer_location += upload.size;
        }

        //Reallocate upload buffer if it's too small
        const GLsizeiptr staging_buffer_size = staging_buffer.get_slice_size();
        if (staging_buffer_location > staging_buffer_size || force_reallocate_staging_buffer)
        {
            reallocate_staging_buffer(staging_buffer_location);
        }


        GLbyte* upload_buffer_pointer = static_cast<GLbyte*>(staging_buffer.get_mapped_slice_pointer());

        for (const upload_info& upload : pending_uploads)
        {
            const GLbyte* source_pointer = static_cast<const GLbyte*>(upload.source);
            memcpy(upload_buffer_pointer + upload.upload_buffer_address, source_pointer, upload.size);
        }

        for (const upload_info& upload : pending_uploads)
        {
            pool_buffer.copy_buffer_range(staging_buffer.get_buffer_id(), staging_buffer.get_slice_offset() + upload.upload_buffer_address, upload.write_address, upload.size);
        }

        pending_uploads.clear();
    }

    void pool_data_buffer::block_ready()
    {
        ZoneScoped;
        staging_buffer.block_ready();
        current_staging_buffer_address = 0;
    }

    void pool_data_buffer::upload(const u32& address, const void* const data, const u32 size)
    {
        ZoneScoped;
        const u32 upload_offset = current_staging_buffer_address;
        current_staging_buffer_address += size;

        const GLsizeiptr staging_buffer_size = staging_buffer.get_slice_size();
        if (current_staging_buffer_address > staging_buffer_size || force_reallocate_staging_buffer)
        {
            reallocate_staging_buffer(current_staging_buffer_address * 2);
            current_staging_buffer_address = 0;
        }

        GLbyte* upload_buffer_pointer = static_cast<GLbyte*>(staging_buffer.get_mapped_slice_pointer());
        const GLbyte* source_pointer = static_cast<const GLbyte*>(data);
        memcpy(upload_buffer_pointer + upload_offset, source_pointer, size);

        pool_buffer.copy_buffer_range(staging_buffer.get_buffer_id(), staging_buffer.get_slice_offset() + upload_offset, address, size);
    }

    void pool_data_buffer::reallocate_staging_buffer(const GLsizeiptr size)
    {
        ZoneScoped;
        log_info(logger_tag, "Allocating pool staging buffer to ", stringify_bytes(size), ".");
        staging_buffer.free();
        staging_buffer.allocate_for_cpu_writes(pool_buffer.get_target(), size);
        force_reallocate_staging_buffer = false;
        log_info(logger_tag, "Buffer ", staging_buffer.get_buffer_id(), " allocated as pool staging buffer.");
    }
}
