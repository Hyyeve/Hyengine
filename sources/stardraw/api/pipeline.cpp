#include "pipeline.hpp"

namespace stardraw
{
    pipeline_ptr pipeline::create()
    {
        return pipeline_ptr(new pipeline());
    }

    pipeline::~pipeline()
    {
        delete backend;
    }

    status pipeline::create_backend(const graphics_api api)
    {
        if (backend != nullptr)
        {
            return status::ALREADY_INITIALIZED;
        }

        backend = api_backend::create(api);

        if (backend == nullptr)
        {
            return status::UNSUPPORTED;
        }

        return status::SUCCESS;
    }

    status pipeline::execute_command(const command* cmd) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        if (cmd == nullptr) return status::UNEXPECTED_NULL;
        return backend->execute_cmd(cmd);
    }

    status pipeline::execute_command_list(const std::string_view& name) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        return backend->execute_cmd_list(name);
    }

    status pipeline::execute_temp_command_list(command_list_ptr cmd_list) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        if (cmd_list == nullptr) return status::UNEXPECTED_NULL;
        return backend->execute_temp_cmd_list(std::move(cmd_list));
    }

    status pipeline::attach_command_list(const std::string_view& list_name, command_list_ptr cmd_list) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        if (cmd_list == nullptr) return status::UNEXPECTED_NULL;
        return backend->attach_command_list(list_name, std::move(cmd_list));
    }

    status pipeline::attach_descriptor_list(const std::string_view& list_name, descriptor_list_ptr descriptors) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        if (descriptors == nullptr) return status::UNEXPECTED_NULL;
        return backend->attach_descriptor_list(list_name, std::move(descriptors));
    }

    status pipeline::delete_command_list(const std::string_view& list_name) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        return backend->delete_command_list(list_name);
    }

    status pipeline::delete_descriptor_list(const std::string_view& list_name) const
    {
        if (backend == nullptr) return status::NOT_INITIALIZED;
        return backend->delete_descriptor_list(list_name);
    }
}
