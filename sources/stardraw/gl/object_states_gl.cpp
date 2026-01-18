#include "object_states_gl.hpp"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

namespace stardraw
{
    gl_buffer_state::gl_buffer_state(buffer_descriptor* desc) : descriptor(desc)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Create buffer object");

        if (descriptor->buff_type != buffer_type::STREAMING && desc->buff_type != buffer_type::PERSISTANT)
        {
            descriptor = nullptr;
            return;
        }

        is_streaming_buffer = descriptor->buff_type == buffer_type::STREAMING;

        if (is_streaming_buffer)
        {
            slice_size = descriptor->size;
            main_buffer_size = slice_size * buffer_slices.size();
            current_slice_index = 0;

            GLsizeiptr slice_offset = 0;
            for (slice& slice : buffer_slices)
            {
                slice.start_address = slice_offset;
                slice_offset += slice_size;
            }
        }
        else
        {
            main_buffer_size = descriptor->size;
            //Slices will be setup by staging buffer allocation later
        }

        glCreateBuffers(1, &main_buffer_id);

        if (main_buffer_id == 0)
        {
            descriptor = nullptr;
            return;
        }

        constexpr GLbitfield mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(main_buffer_id, main_buffer_size, nullptr, is_streaming_buffer ? mapping_flags : 0);
    }

    gl_buffer_state::~gl_buffer_state()
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Delete buffer object");
        if (is_streaming_buffer)
        {
            if (write_pointer != nullptr)
            {
                glUnmapNamedBuffer(main_buffer_id);
                write_pointer = nullptr;
            }

            glDeleteBuffers(1, &main_buffer_id);
        }
        else
        {
            if (write_pointer != nullptr)
            {
                glUnmapNamedBuffer(staging_buffer_id);
                write_pointer = nullptr;
            }

            if (staging_buffer_id != 0)
            {
                glDeleteBuffers(1, &staging_buffer_id);
            }

            glDeleteBuffers(1, &main_buffer_id);
        }

        delete descriptor;
    }

    descriptor_type gl_buffer_state::object_type() const
    {
        return descriptor_type::BUFFER;
    }

    bool gl_buffer_state::is_valid() const
    {
        const bool descriptor_ok = descriptor != nullptr;
        const bool buffers_ok = main_buffer_id != 0 && (is_streaming_buffer ? true : staging_buffer_id != 0);
        const bool mapping_ok = write_pointer != nullptr;

        return descriptor_ok && buffers_ok && mapping_ok;
    }

    status gl_buffer_state::bind_to(const GLenum target) const
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Bind buffer");
        glBindBuffer(target, main_buffer_id);
        return status::SUCCESS;
    }

    status gl_buffer_state::bind_to_slot(const GLenum target, const GLuint slot) const
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Bind buffer (slot binding)");
        glBindBufferRange(target, slot, main_buffer_id, get_start_offset(), slice_size);
        return status::SUCCESS;
    }

    status gl_buffer_state::bind_to_slot(const GLenum target, const GLuint slot, const GLintptr offset, const GLsizeiptr bytes) const
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Bind buffer (slot binding)");
        if (offset + bytes > slice_size) return status::RANGE_OVERFLOW;
        glBindBufferRange(target, slot, main_buffer_id, get_start_offset() + offset, bytes);
        return status::SUCCESS;
    }

    status gl_buffer_state::sync_buffer()
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Sync buffer for upload");

        //Place new sync point
        GLsync& new_sync = buffer_slices[current_slice_index].fence;
        if (new_sync != nullptr) { glDeleteSync(new_sync); }
        new_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        //Move to next slice
        current_slice_index = (current_slice_index + 1) % buffer_slices.size();
        current_staging_buff_address = 0;

        //Wait for slice to become available
        const GLsync& wait_sync = buffer_slices[current_slice_index].fence;
        if (wait_sync == nullptr) return status::SUCCESS;
        const GLenum wait_return = glClientWaitSync(wait_sync, 0, GL_TIMEOUT_IGNORED);
        if (wait_return == GL_ALREADY_SIGNALED || wait_return == GL_CONDITION_SATISFIED) return status::SUCCESS;
        return status::TIMEOUT;
    }

    status gl_buffer_state::upload_data(const GLintptr address, const void* const data, const GLintptr bytes)
    {
        if (is_streaming_buffer) return upload_data_streamed(address, data, bytes);
        return upload_data_staged(address, data, bytes);
    }

    status gl_buffer_state::copy_data(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Buffer data transfer");

        if (!is_in_buffer_range(write_offset, bytes)) return status::RANGE_OVERFLOW;
        glCopyNamedBufferSubData(source_buffer_id, main_buffer_id, read_offset, get_start_offset() + write_offset, bytes);
        return status::SUCCESS;
    }

    GLintptr gl_buffer_state::get_start_offset() const
    {
        if (is_streaming_buffer) return buffer_slices[current_slice_index].start_address;
        return 0;
    }

    GLsizeiptr gl_buffer_state::get_usable_size() const
    {
        return is_streaming_buffer ? slice_size : main_buffer_size;
    }

    bool gl_buffer_state::is_in_buffer_range(const GLintptr address, const GLsizeiptr size) const
    {
        return address + size <= get_usable_size();
    }

    GLuint gl_buffer_state::transfer_source_gl_id() const
    {
        return main_buffer_id;
    }

    status gl_buffer_state::upload_data_streamed(const GLintptr address, const void* const data, const GLsizeiptr bytes) const
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Streaming buffer upload")
        if (!is_in_buffer_range(address, bytes)) return status::RANGE_OVERFLOW;
        const GLbyte* source_pointer = static_cast<const GLbyte*>(data);
        GLbyte* dest_pointer = static_cast<GLbyte*>(write_pointer) + get_start_offset();
        memcpy(dest_pointer + address, source_pointer, bytes);
        return status::SUCCESS;
    }

    status gl_buffer_state::upload_data_staged(const GLintptr address, const void* const data, const GLintptr bytes)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Persistant buffer upload")
        const GLsizeiptr upload_offset = current_staging_buff_address;
        current_staging_buff_address += bytes;

        if (current_staging_buff_address > slice_size || staging_buffer_id == 0)
        {
            const status realloc_status = prepare_staging_buffer(current_staging_buff_address * 2);
            if (realloc_status != status::SUCCESS)
            {
                current_staging_buff_address -= bytes;
                return realloc_status;
            }
            current_staging_buff_address = 0;
        }

        GLbyte* upload_buffer_pointer = static_cast<GLbyte*>(write_pointer) + get_start_offset();
        const GLbyte* source_pointer = static_cast<const GLbyte*>(data);
        memcpy(upload_buffer_pointer + upload_offset, source_pointer, bytes);

        return copy_data(staging_buffer_id, get_start_offset() + upload_offset, address, bytes);
    }

    status gl_buffer_state::prepare_staging_buffer(const GLsizeiptr size)
    {
        ZoneScoped;
        TracyGpuZone("[Stardraw] Allocate staging buffer");

        //Deallocate previous buffer if one exists
        {
            if (write_pointer != nullptr)
            {
                glUnmapNamedBuffer(staging_buffer_id);
                write_pointer = nullptr;
            }

            if (staging_buffer_id != 0)
            {
                glDeleteBuffers(1, &staging_buffer_id);
            }
        }

        slice_size = size;
        const GLsizeiptr total_size = size * buffer_slices.size();
        current_slice_index = 0;

        GLsizeiptr slice_offset = 0;
        for (slice& slice : buffer_slices)
        {
            slice.start_address = slice_offset;
            slice_offset += slice_size;
        }

        glCreateBuffers(1, &staging_buffer_id);
        if (staging_buffer_id == 0) return status::BACKEND_FAILURE;

        constexpr GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(staging_buffer_id, total_size, nullptr, flags);

        write_pointer = glMapNamedBufferRange(staging_buffer_id, 0, total_size, flags);
        if (write_pointer == nullptr) return status::BACKEND_FAILURE;

        return status::SUCCESS;
    }
}
