#pragma once
#include <string_view>
#include <unordered_map>

#include "object_states_gl.hpp"
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

        [[nodiscard]] status execute_command(const command* cmd) override;
        [[nodiscard]] status execute_command_buffer(const std::string_view& name) override;
        [[nodiscard]] status execute_temp_command_buffer(command_list_ptr commands) override;
        [[nodiscard]] status create_command_buffer(const std::string_view& name, command_list_ptr commands) override;
        [[nodiscard]] status delete_command_buffer(const std::string_view& name) override;
        [[nodiscard]] status create_objects(descriptor_list_ptr descriptors) override;
        [[nodiscard]] status delete_object(const std::string_view& name) override;
        [[nodiscard]] signal_status check_signal(const std::string_view& name) override;
        [[nodiscard]] signal_status wait_signal(const std::string_view& name, uint64_t timeout) override;

    private:
        [[nodiscard]] status bind_buffer(const object_identifier& source, GLenum target);

        [[nodiscard]] status execute_draw_cmd(const draw_command* cmd);
        [[nodiscard]] status execute_draw_indexed(const draw_indexed_command* cmd);
        [[nodiscard]] status execute_draw_indirect(const draw_indirect_command* cmd);
        [[nodiscard]] status execute_draw_indexed_indirect(const draw_indexed_indirect_command* cmd);

        [[nodiscard]] status execute_buffer_upload(const buffer_upload_command* cmd);
        [[nodiscard]] status execute_buffer_copy(const buffer_copy_command* cmd);
        [[nodiscard]] status execute_buffer_attach(const buffer_attach_command* cmd);

        [[nodiscard]] static status execute_config_blending(const config_blending_command* cmd);
        [[nodiscard]] static status execute_config_stencil(const config_stencil_command* cmd);
        [[nodiscard]] static status execute_config_scissor(const config_scissor_command* cmd);
        [[nodiscard]] static status execute_config_face_cull(const config_face_cull_command* cmd);
        [[nodiscard]] static status execute_config_depth_test(const config_depth_test_command* cmd);
        [[nodiscard]] static status execute_config_depth_range(const config_depth_range_command* cmd);

        [[nodiscard]] static status execute_clear_window(const clear_window_command* cmd);

        [[nodiscard]] status create_object(const descriptor* descriptor);

        [[nodiscard]] status create_buffer_state(const buffer_descriptor* descriptor);
        [[nodiscard]] status create_vertex_specification_state(const vertex_specification_descriptor* descriptor);
        [[nodiscard]] status bind_vertex_specification_state(const object_identifier& source, GLsizeiptr& out_index_buffer_offset);

        template <typename state_type, descriptor_type object_type>
        [[nodiscard]] state_type* find_gl_state(const object_identifier& identifier)
        {
            static_assert(std::is_base_of_v<gl_object_state, state_type>);
            if (objects.contains(identifier.hash))
            {
                gl_object_state* identified_state = objects[identifier.hash];
                if (identified_state->object_type() == object_type)
                {
                    return dynamic_cast<state_type*>(identified_state);
                }
            }

            return nullptr;
        }

        [[nodiscard]] inline gl_buffer_state* find_gl_buffer_state(const object_identifier& identifier)
        {
            return find_gl_state<gl_buffer_state, descriptor_type::BUFFER>(identifier);
        }

        [[nodiscard]] inline gl_vertex_specification_state* find_gl_vertex_specification_state(const object_identifier& identifier)
        {
            return find_gl_state<gl_vertex_specification_state, descriptor_type::VERTEX_SPECIFICATION>(identifier);
        }

        std::unordered_map<std::string, command_list_ptr> command_lists;
        std::unordered_map<uint64_t, gl_object_state*> objects;
        std::unordered_map<std::string, gl_signal_state> signals;
    };
}
