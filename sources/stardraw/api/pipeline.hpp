#pragma once

#include <string_view>

#include "api_backend.hpp"
#include "polymorphic_list.hpp"

namespace stardraw
{
    class pipeline;
    typedef std::unique_ptr<pipeline> pipeline_ptr;

    class pipeline
    {
    public:
        static pipeline_ptr create();
        ~pipeline();

        pipeline(pipeline& other) = delete;                      //COPY CONSTRUCTOR
        pipeline(pipeline&& other) = delete;                     //MOVE CONSTRUCTOR
        pipeline& operator=(pipeline& other) = delete;           //COPY ASSIGNMENT
        pipeline& operator=(pipeline&& other) noexcept = delete; //MOVE ASSIGNMENT

        status create_backend(graphics_api api);

        [[nodiscard]] status execute_command(const command* cmd) const;
        [[nodiscard]] status execute_command_list(const std::string_view& name) const;
        [[nodiscard]] status execute_temp_command_list(command_list_ptr cmd_list) const;

        [[nodiscard]] status attach_command_list(const std::string_view& list_name, command_list_ptr cmd_list) const;
        [[nodiscard]] status attach_descriptor_list(const std::string_view& list_name, const descriptor_list_ptr descriptors) const;

        [[nodiscard]] status delete_command_list(const std::string_view& list_name) const;
        [[nodiscard]] status delete_descriptor_list(const std::string_view& list_name) const;

        template <typename... command_types>
        [[nodiscard]] status attach_commands(const std::string_view& name, const command_types&... commands)
        {
            command_list_builder builder;
            (builder.add(std::forward<const command_types&...>(commands)), ...);
            command_list_ptr list = builder.finish();
            return attach_command_list(name, std::move(list));
        }

        template <typename... descriptor_types>
        [[nodiscard]] status attach_descriptors(const std::string_view& name, const descriptor_types... commands)
        {
            descriptor_list_builder builder;
            (builder.add(std::forward<descriptor_types...>(commands)), ...);
            descriptor_list_ptr list = builder.finish();
            return attach_descriptor_list(name, std::move(list));
        }

    private:
        pipeline() = default;
        api_backend* backend = nullptr;
    };
}
