#include "backend_impl_gl.hpp"
namespace stardraw
{
    [[nodiscard]] status backend_impl_gl::execute_cmd(const command* cmd)
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
        }

        return status::UNSUPPORTED;
    }

    status backend_impl_gl::execute_cmd_list(const std::string_view& name)
    {
        return status::UNIMPLEMENTED; //TODO
    }

    [[nodiscard]] status backend_impl_gl::execute_temp_cmd_list(const command_list_ptr commands)
    {
        for (const command* cmd : *commands)
        {
            const status result = execute_cmd(cmd);
            if (result != status::SUCCESS) return result;
        }

        return status::SUCCESS;
    }

    status backend_impl_gl::attach_command_list(std::string_view list_name, command_list_ptr commands)
    {
        return status::UNIMPLEMENTED; //TODO
    }

    status backend_impl_gl::attach_descriptor_list(std::string_view list_name, descriptor_list_ptr descriptors)
    {
        return status::UNIMPLEMENTED;
    }

    status backend_impl_gl::delete_command_list(const std::string_view& list_name)
    {
        return status::UNIMPLEMENTED;
    }

    status backend_impl_gl::delete_descriptor_list(const std::string_view& list_name)
    {
        return status::UNIMPLEMENTED;
    }

    status backend_impl_gl::bind_vertex_buffer(const std::string_view& source)
    {
        return status::UNIMPLEMENTED; //TODO
    }

    status backend_impl_gl::bind_index_buffer(const std::string_view& source)
    {
        return status::UNIMPLEMENTED; //TODO
    }

    status backend_impl_gl::bind_indirect_buffer(const std::string_view& source)
    {
        return status::UNIMPLEMENTED; //TODO
    }
}