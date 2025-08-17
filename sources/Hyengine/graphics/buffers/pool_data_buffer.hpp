#pragma once

#include <future>
#include <memory>
#include <vector>

#include "standard_data_buffer.hpp"
#include "../../common/pool_allocation_tracker.hpp"
#include "../../library/gl.hpp"
#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    using namespace hyengine;

    class pool_data_buffer
    {
    public:
        pool_data_buffer(const pool_data_buffer& other) = delete; //COPY CONSTRUCTOR
        pool_data_buffer(pool_data_buffer&& other) = delete; //MOVE CONSTRUCTOR
        pool_data_buffer& operator=(const pool_data_buffer& other) = delete; //COPY ASSIGNMENT
        pool_data_buffer& operator=(pool_data_buffer&& other) noexcept = delete; //MOVE ASSIGNMENT

        explicit pool_data_buffer();
        ~pool_data_buffer();

        void allocate(const GLenum target, const GLsizeiptr size, const GLsizeiptr staging_size);
        void free();

        void shrink_staging_buffer();
        void reserve_staging_buffer_size(const u32 size);

        bool try_allocate_space(const u32 size, u32& address);
        void deallocate_space(const u32 address);
        [[nodiscard]] u32 get_last_allocated_address() const;

    private:
        //Caller needs to ensure data pointer remains valid until uploads are submitted!
        void queue_upload(const u32& address, const void* const data, const u32 size);
        void submit_uploads();
    public:

        void block_ready();
        void upload(const u32& address, const void* const data, const u32 size);
        //void upload_async(const u32& address, const void* const data, const u32 size);

        void bind_state() const;
        void unbind_state() const;

        void bind_buffer_base(const i32 binding) const;
        void bind_buffer_range(const i32 binding, const GLintptr offset, const GLsizeiptr size) const;

        [[nodiscard]] GLsizeiptr get_size() const;
        [[nodiscard]] GLuint get_buffer_id() const;
        [[nodiscard]] GLenum get_target() const;
        [[nodiscard]] u32 get_pending_upload_count() const;

    private:
        void reallocate_staging_buffer(const GLsizeiptr size);

        struct upload_info
        {
            const void* source;
            GLintptr upload_buffer_address;
            GLintptr write_address;
            GLsizeiptr size;
        };

        const std::string logger_tag = "GPU Pool Buffer";

        standard_data_buffer pool_buffer;
        pool_allocation_tracker pool_allocator;

        standard_data_buffer staging_buffer;
        u32 current_staging_buffer_address;
        std::vector<upload_info> pending_uploads;
        bool force_reallocate_staging_buffer = false;
    };
}
