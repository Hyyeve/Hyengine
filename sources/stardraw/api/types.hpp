#pragma once
#include <cstdint>
#include <memory>

#include "polymorphic_list.hpp"

namespace stardraw
{
    enum class graphics_api : uint8_t
    {
        GL,
    };

    enum class status : uint8_t
    {
        SUCCESS, UNSUPPORTED, UNIMPLEMENTED,
        ALREADY_INITIALIZED,
        NOT_INITIALIZED,
        PARAM_NOT_INITIALIZED,
        UNEXPECTED_NULL,
    };

    enum class descriptor_type : uint8_t
    {
        BUFFER,
    };

    enum class command_type : uint8_t
    {
        DRAW, DRAW_INDIRECT, DRAW_INDEXED, DRAW_INDEXED_INDIRECT,
        CONFIG_BLENDING, CONFIG_STENCIL,
    };

    struct descriptor
    {
        virtual ~descriptor() = 0;

        [[nodiscard]] virtual descriptor_type type() = 0;
    };

    struct command
    {
        virtual ~command() = default;

        [[nodiscard]] virtual command_type type() const = 0;
    };

    typedef polymorphic_list<command> command_list;
    typedef std::unique_ptr<const command_list> command_list_ptr;
    typedef polymorphic_list_builder<command> command_list_builder;
    typedef polymorphic_list<descriptor> descriptor_list;
    typedef std::unique_ptr<const descriptor_list> descriptor_list_ptr;
    typedef polymorphic_list_builder<descriptor> descriptor_list_builder;
}
