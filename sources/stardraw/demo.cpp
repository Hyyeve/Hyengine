#include "api/commands.hpp"
#include "api/pipeline.hpp"

using namespace stardraw;

int main()
{
    command_list_ptr cmd_buff = command_list::create(
    );

    command_list_builder builder;
    command_list_ptr built_list = builder.finish();

    pipeline_ptr test_pipeline = pipeline::create();
    test_pipeline->create_backend(graphics_api::GL);

    test_pipeline->attach_command_list("cmdbuff1", std::move(cmd_buff));
    test_pipeline->attach_command_list("cmdbuff2", std::move(built_list));
    test_pipeline->attach_commands(
        "cmdbuff3"
    );

    return 0;
}
