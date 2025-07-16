#include "pool_data_buffer.hpp"

#include <tracy/Tracy.hpp>

#include "../../core/logger.hpp"


namespace hyengine::graphics {

    using namespace hyengine;

    pool_data_buffer::pool_data_buffer(): pool_allocator(0), temp_upload_buffer(nullptr), temp_upload_buffer_size(0), temp_upload_buffer_offset(0)
    {
    }

    pool_data_buffer::~pool_data_buffer()
    {
        ZoneScoped;
        free();
    }

    void pool_data_buffer::allocate(const GLenum target, const GLsizeiptr size)
    {
        ZoneScoped;
        pool_buffer.allocate_for_gpu_writes(target, size);
        pool_allocator = common::pool_allocation_tracker(size);
        //We don't know how much we'll need to upload at once, so don't allocate the upload buffer yet
        logger::message_info(logger::format("Buffer ", pool_buffer.get_buffer_id(), " allocated as pool buffer."), logger_tag);
    }

    void pool_data_buffer::free()
    {
        ZoneScoped;
        pool_buffer.free();
        staging_buffer.free();
    }

    void pool_data_buffer::shrink_staging_buffer()
    {
        ZoneScoped;
        force_reallocate_staging_buffer = true;
    }

    bool pool_data_buffer::try_allocate_space(const unsigned int size, unsigned int& address)
    {
        ZoneScoped;
        return pool_allocator.try_allocate(size, address);
    }

    void pool_data_buffer::deallocate_space(const unsigned int address)
    {
        ZoneScoped;
        pool_allocator.deallocate(address);
    }

    unsigned int pool_data_buffer::get_last_allocated_address() const
    {
        ZoneScoped;
        return pool_allocator.get_last_used_address();
    }

    void pool_data_buffer::queue_upload(const unsigned int& address, const void* const data, const unsigned int size)
    {
        ZoneScoped;
        const unsigned int temp_address = temp_upload_buffer_offset;
        const bool has_temp_space = temp_upload_buffer_offset + size <= temp_upload_buffer_size;

        if (!has_temp_space)
        {
            const unsigned int new_buffer_size = temp_upload_buffer_size * 2 + size;
            GLbyte* new_temp_memory = new GLbyte[new_buffer_size];

            if (temp_upload_buffer != nullptr)
            {
                memcpy(new_temp_memory, temp_upload_buffer, temp_upload_buffer_size);
                delete[] temp_upload_buffer;
            }

            temp_upload_buffer = new_temp_memory;
            temp_upload_buffer_size = new_buffer_size;
        }

        temp_upload_buffer_offset += size;

        memcpy(temp_upload_buffer + temp_address, data, size);
        pending_uploads.push_back({temp_address, 0, address, size});
    }

    void pool_data_buffer::bind_state() const
    {
        ZoneScoped;
        pool_buffer.bind_state();
    }

    void pool_data_buffer::unbind_state() const
    {
        ZoneScoped;
        pool_buffer.unbind_state();
    }

    void pool_data_buffer::bind_buffer_base(const int binding) const
    {
        ZoneScoped;
        pool_buffer.bind_buffer_base(binding);
    }

    void pool_data_buffer::bind_buffer_range(const int binding, const GLintptr offset, const GLsizeiptr size) const
    {
        ZoneScoped;
        pool_buffer.bind_buffer_range(binding, offset, size);
    }

    GLsizeiptr pool_data_buffer::get_size() const
    {
        ZoneScoped;
        return pool_buffer.get_size();
    }

    GLuint pool_data_buffer::get_buffer_id() const
    {
        ZoneScoped;
        return pool_buffer.get_buffer_id();
    }

    GLenum pool_data_buffer::get_target() const
    {
        ZoneScoped;
        return pool_buffer.get_target();
    }

    unsigned int pool_data_buffer::get_pending_upload_count() const
    {
        ZoneScoped;
        return pending_uploads.size();
    }

    void pool_data_buffer::submit_uploads()
    {
        ZoneScoped;
        if (pending_uploads.empty())
        {
            return;
        }

        unsigned int staging_buffer_location = 0;
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

        staging_buffer.block_ready();

        GLbyte* upload_buffer_pointer = static_cast<GLbyte*>(staging_buffer.get_mapped_slice_pointer());

        for (const upload_info& upload : pending_uploads)
        {
            const GLbyte* temp_buffer_pointer = temp_upload_buffer + upload.temp_data_address;
            memcpy(upload_buffer_pointer + upload.upload_buffer_address, temp_buffer_pointer, upload.size);
        }

        temp_upload_buffer_offset = 0;

        for (const upload_info& upload : pending_uploads)
        {
            pool_buffer.copy_buffer_range(staging_buffer.get_buffer_id(), staging_buffer.get_slice_offset() + upload.upload_buffer_address, upload.write_address, upload.size);
        }

        pending_uploads.clear();
    }

    void pool_data_buffer::reallocate_staging_buffer(const GLsizeiptr size)
    {
        ZoneScoped;
        logger::info(logger_tag, " No space in pool staging buffer, reallocating.");
        staging_buffer.free();
        staging_buffer.allocate_for_cpu_writes(pool_buffer.get_target(), size);
        force_reallocate_staging_buffer = false;
        logger::message_info(logger::format("Buffer ", staging_buffer.get_buffer_id(), " allocated as pool staging buffer."), logger_tag);
    }
}
