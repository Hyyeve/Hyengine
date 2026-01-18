#include "backend_impl_gl.hpp"

namespace stardraw
{
    [[nodiscard]] status backend_impl_gl::execute_command(const command* cmd)
    {
        if (cmd == nullptr)
        {
            return status::UNEXPECTED_NULL;
        }

        const command_type type = cmd->type();
        switch (type)
        {
            case command_type::DRAW: return execute_draw_cmd(dynamic_cast<const draw_command*>(cmd));
            case command_type::DRAW_INDEXED: return execute_draw_indexed(dynamic_cast<const draw_indexed_command*>(cmd));
            case command_type::DRAW_INDIRECT: return execute_draw_indirect(dynamic_cast<const draw_indirect_command*>(cmd));
            case command_type::DRAW_INDEXED_INDIRECT: return execute_draw_indexed_indirect(dynamic_cast<const draw_indexed_indirect_command*>(cmd));

            case command_type::CONFIG_BLENDING: return execute_config_blending(dynamic_cast<const config_blending_command*>(cmd));
            case command_type::CONFIG_STENCIL: return execute_config_stencil(dynamic_cast<const config_stencil_command*>(cmd));

            case command_type::BUFFER_SYNC: return execute_buffer_sync(dynamic_cast<const buffer_sync_command*>(cmd));
            case command_type::BUFFER_UPLOAD: return execute_buffer_upload(dynamic_cast<const buffer_upload_command*>(cmd));
            case command_type::BUFFER_COPY: return execute_buffer_copy(dynamic_cast<const buffer_copy_command*>(cmd));
            case command_type::BUFFER_DOWNLOAD: return status::UNSUPPORTED; //TODO
            case command_type::BUFFER_ATTACH: return execute_buffer_attach(dynamic_cast<const buffer_attach_command*>(cmd));
        }

        return status::UNSUPPORTED;
    }

    [[nodiscard]] status backend_impl_gl::execute_command_buffer(const std::string_view& name)
    {
        //Opengl doesn't have any persistant command buffers, so we just execute it like a temporary one without consuming it.
        if (!command_lists.contains(std::string(name))) return status::UNKNOWN_NAME;
        const command_list_ptr& refren = command_lists[std::string(name)];

        for (const command* cmd : *refren.get())
        {
            const status result = execute_command(cmd);
            if (result != status::SUCCESS) return result;
        }

        return status::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::execute_temp_command_buffer(const command_list_ptr commands)
    {
        for (const command* cmd : *commands)
        {
            const status result = execute_command(cmd);
            if (result != status::SUCCESS) return result;
        }

        return status::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::create_command_buffer(const std::string_view& name, command_list_ptr commands)
    {
        if (command_lists.contains(std::string(name))) return status::DUPLICATE_NAME;
        command_lists[std::string(name)] = std::move(commands);
        return status::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::delete_command_buffer(const std::string_view& name)
    {
        if (!command_lists.contains(std::string(name))) return status::NOTHING_TO_DO;
        command_list_ptr cmd_list = std::move(command_lists[std::string(name)]);
        command_lists.erase(std::string(name));
        cmd_list.reset();
        return status::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::create_objects(descriptor_list_ptr descriptors)
    {
        return status::UNIMPLEMENTED;
    }

    [[nodiscard]] status backend_impl_gl::delete_object(const std::string_view& name)
    {
        return status::UNIMPLEMENTED;
    }

    [[nodiscard]] signal_status backend_impl_gl::check_signal(const std::string_view& name)
    {
        return wait_signal(name, 0);
    }

    [[nodiscard]] signal_status backend_impl_gl::wait_signal(const std::string_view& name, const uint64_t timeout)
    {
        if (!signals.contains(std::string(name)))
        {
            return signal_status::UNKNOWN_SIGNAL;
        }

        const gl_signal_state& state = signals[std::string(name)];
        const GLenum status = glClientWaitSync(state.sync_point, 0, timeout);
        switch (status)
        {
            case GL_ALREADY_SIGNALED: return signal_status::SIGNALLED;
            case GL_TIMEOUT_EXPIRED: return signal_status::TIMED_OUT;
            case GL_CONDITION_SATISFIED: return signal_status::SIGNALLED;
            default: return signal_status::NOT_SIGNALLED;
        }
    }

    [[nodiscard]] status backend_impl_gl::bind_buffer(const object_identifier& source, const GLenum target)
    {
        const gl_buffer_state* buffer_state = find_gl_buffer_state(source);
        if (buffer_state == nullptr) return status::UNKNOWN_SOURCE;
        return buffer_state->bind_to(target);
    }
}
