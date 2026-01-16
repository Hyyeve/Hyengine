#pragma once
#include <string_view>
#include <unordered_map>

#include "types_gl.hpp"
#include "stardraw/api/api_backend.hpp"
#include "stardraw/api/commands.hpp"
#include "stardraw/api/types.hpp"

namespace stardraw
{
    class backend_impl_gl final : public api_backend
    {
    public:
        backend_impl_gl() = default;
        ~backend_impl_gl() override = default;

        [[nodiscard]] status execute_cmd(const command* cmd) override;
        [[nodiscard]] status execute_cmd_list(const std::string_view& name) override;
        [[nodiscard]] status execute_temp_cmd_list(command_list_ptr commands) override;
        [[nodiscard]] status attach_command_list(std::string_view list_name, command_list_ptr commands) override;
        [[nodiscard]] status attach_descriptor_list(std::string_view list_name, descriptor_list_ptr descriptors) override;
        [[nodiscard]] status delete_command_list(const std::string_view& list_name) override;
        [[nodiscard]] status delete_descriptor_list(const std::string_view& list_name) override;

    private:
        status bind_vertex_buffer(const std::string_view& source);
        status bind_index_buffer(const std::string_view& source);
        status bind_indirect_buffer(const std::string_view& source);

        status execute_draw_cmd(const draw_command* cmd);
        status execute_draw_indexed(const draw_indexed_command* cmd);
        status execute_draw_indirect(const draw_indirect_command* cmd);
        status execute_draw_indexed_indirect(const draw_indexed_indirect_command* cmd);

        static status execute_config_blending(const config_blending_command* cmd);
        static status execute_config_stencil(const config_stencil_command* cmd);

        std::unordered_map<std::string, gl_object_state*> objects;
    };
}
