#pragma once
#include <string_view>

#include "polymorphic_list.hpp"
#include "types.hpp"

namespace stardraw
{
    class api_backend
    {
    public:
        static api_backend* create(const graphics_api api);
        virtual ~api_backend() = default;
        [[nodiscard]] virtual status execute_cmd(const command* cmd) = 0;
        [[nodiscard]] virtual status execute_cmd_list(const std::string_view& name) = 0;
        [[nodiscard]] virtual status execute_temp_cmd_list(command_list_ptr commands) = 0;
        [[nodiscard]] virtual status attach_command_list(std::string_view list_name, command_list_ptr commands) = 0;
        [[nodiscard]] virtual status attach_descriptor_list(std::string_view list_name, descriptor_list_ptr descriptors) = 0;
        [[nodiscard]] virtual status delete_command_list(const std::string_view& list_name) = 0;
        [[nodiscard]] virtual status delete_descriptor_list(const std::string_view& list_name) = 0;
    };
}
