#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "polymorphic_list.hpp"

namespace stardraw
{
    enum class graphics_api : uint8_t
    {
        GL,
    };

    enum class signal_status : uint8_t
    {
        SIGNALLED, NOT_SIGNALLED, TIMED_OUT, UNKNOWN_SIGNAL
    };

    enum class status_type : uint8_t
    {
        SUCCESS, UNSUPPORTED, UNIMPLEMENTED, NOTHING_TO_DO,
        ALREADY_INITIALIZED, NOT_INITIALIZED,
        UNKNOWN_SOURCE, UNEXPECTED_NULL, RANGE_OVERFLOW, TIMEOUT,
        DUPLICATE_NAME, UNKNOWN_NAME,
        BACKEND_FAILURE, BROKEN_SOURCE,
    };

    struct status
    {
        // ReSharper disable once CppNonExplicitConvertingConstructor
        status(const status_type type) : type{type}, message("No message provided") {}
        status(const status_type type, const std::string_view& message) : type{type}, message(message) {}

        status_type type;
        std::string message;
    };

    enum class descriptor_type : uint8_t
    {
        BUFFER, VERTEX_SPECIFICATION,
    };

    enum class command_type : uint8_t
    {
        DRAW, DRAW_INDIRECT, DRAW_INDEXED, DRAW_INDEXED_INDIRECT,
        CONFIG_BLENDING, CONFIG_STENCIL, CONFIG_SCISSOR, CONFIG_FACE_CULL, CONFIG_DEPTH_TEST, CONFIG_DEPTH_RANGE,
        BUFFER_UPLOAD, BUFFER_COPY, BUFFER_DOWNLOAD, BUFFER_ATTACH,
        CLEAR_WINDOW, CLEAR_BUFFER
    };

    inline bool is_error_status(const status& status)
    {
        switch (status.type)
        {
            case status_type::NOTHING_TO_DO:
            case status_type::SUCCESS: return false;
            default: return true;
        }
    }

    struct object_identifier
    {
        explicit object_identifier(const std::string_view& string) : hash(std::hash<std::string_view>()(string)), name(string) {}
        uint64_t hash;
        std::string name;
    };


    struct descriptor
    {
        explicit descriptor(const std::string_view& name) : ident(name) {}
        virtual ~descriptor() = default;

        [[nodiscard]] virtual descriptor_type type() const = 0;
        [[nodiscard]] const object_identifier& identifier() const;

    private:
        object_identifier ident;
    };

    inline const object_identifier& descriptor::identifier() const
    {
        return ident;
    }

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

