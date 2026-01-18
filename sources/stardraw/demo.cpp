#include <array>
#include <chrono>
#include <iostream>
#include <unordered_map>

#include "api/commands.hpp"
#include "api/pipeline.hpp"

using namespace stardraw;

int main()
{
    command_list_ptr cmd_buff = command_list::create(
        config_blending_command(standard_blending_configs::ALPHA)
    );

    command_list_builder builder;
    builder.add(config_blending_command(standard_blending_configs::ADDITIVE));
    command_list_ptr built_list = builder.finish();

    pipeline_ptr test_pipeline = pipeline::create();
    test_pipeline->create_backend(graphics_api::GL);

    test_pipeline->create_command_buffer("cmdbuff1", std::move(cmd_buff));
    test_pipeline->create_command_buffer("cmdbuff2", std::move(built_list));
    test_pipeline->create_command_buffer_from_commands(
        "cmdbuff3",
        config_blending_command(standard_blending_configs::MULTIPLY)
    );

    return 0;
}
