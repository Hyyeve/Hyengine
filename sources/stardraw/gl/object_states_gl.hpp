#pragma once
#include <array>

#include "types_gl.hpp"
#include "glad/glad.h"

namespace stardraw
{
    class gl_buffer_state final : gl_object_state
    {
    public:
        explicit gl_buffer_state(buffer_descriptor* desc);
        ~gl_buffer_state() override;

        [[nodiscard]] descriptor_type object_type() const override;

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] status bind_to(const GLenum target) const;
        [[nodiscard]] status bind_to_slot(const GLenum target, const GLuint slot) const;
        [[nodiscard]] status bind_to_slot(const GLenum target, const GLuint slot, const GLintptr offset, const GLsizeiptr bytes) const;

        [[nodiscard]] status sync_buffer();
        [[nodiscard]] status upload_data(const GLintptr address, const void* const data, const GLintptr bytes);
        [[nodiscard]] status copy_data(const GLuint source_buffer_id, const GLintptr read_offset, const GLintptr write_offset, const GLintptr bytes) const;

        [[nodiscard]] GLintptr get_start_offset() const;
        [[nodiscard]] GLsizeiptr get_usable_size() const;
        [[nodiscard]] bool is_in_buffer_range(const GLintptr address, const GLsizeiptr size) const;
        [[nodiscard]] GLuint transfer_source_gl_id() const;

    private:
        struct slice
        {
            GLintptr start_address = 0;
            GLsync fence = nullptr;
        };

        [[nodiscard]] status upload_data_streamed(const GLintptr address, const void* const data, GLsizeiptr bytes) const;
        [[nodiscard]] status upload_data_staged(const GLintptr address, const void* const data, const GLintptr bytes);
        [[nodiscard]] status prepare_staging_buffer(const GLsizeiptr size);

        // --- shared data ---
        buffer_descriptor* descriptor = nullptr;
        bool is_streaming_buffer = false;

        GLuint main_buffer_id = 0; //main buffer - either multi-buffered or pool
        void* write_pointer = nullptr; //Either the main buffer or staging buffer depending on buffer setup

        GLsizeiptr main_buffer_size = 0;

        // slices are used for either the main buffer (streaming) or staging buffer (persistant)
        GLsizeiptr slice_size = 0;
        uint32_t current_slice_index = 0;
        std::array<slice, 3> buffer_slices;

        // --- persistent (pool + staging buffer) data ---
        GLuint staging_buffer_id = 0;
        GLsizeiptr current_staging_buff_address = 0;
    };
}
