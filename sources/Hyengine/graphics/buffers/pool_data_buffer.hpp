#pragma once

#include <vector>

#include "standard_data_buffer.hpp"
#include "../../common/pool_allocation_tracker.hpp"
#include "../../library/gl.hpp"

namespace hyengine::graphics {

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

        void allocate(const GLenum target, const GLsizeiptr size);
        void free();

        void shrink_staging_buffer();

        bool try_allocate_space(const unsigned int size, unsigned int& address);
        void deallocate_space(const unsigned int address);
        [[nodiscard]] unsigned int get_last_allocated_address() const;

        //Caller needs to ensure data pointer remains valid until uploads are submitted!
        void queue_upload(const unsigned int& address, const void* const data, const unsigned int size);
        void submit_uploads();

        void bind_state() const;
        void unbind_state() const;

        void bind_buffer_base(const int binding) const;
        void bind_buffer_range(const int binding, const GLintptr offset, const GLsizeiptr size) const;

        [[nodiscard]] GLsizeiptr get_size() const;
        [[nodiscard]] GLuint get_buffer_id() const;
        [[nodiscard]] GLenum get_target() const;
        [[nodiscard]] unsigned int get_pending_upload_count() const;


    private:
        void reallocate_staging_buffer(const GLsizeiptr size);

        struct upload_info
        {
            unsigned int temp_data_address;
            GLintptr upload_buffer_address;
            GLintptr write_address;
            GLsizeiptr size;
        };

        const std::string logger_tag = "GPU Pool Buffer";

        standard_data_buffer pool_buffer;
        common::pool_allocation_tracker pool_allocator;

        standard_data_buffer staging_buffer;
        std::vector<upload_info> pending_uploads;
        bool force_reallocate_staging_buffer = false;

        GLbyte* temp_upload_buffer;
        unsigned int temp_upload_buffer_size;
        unsigned int temp_upload_buffer_offset;
    };
}
