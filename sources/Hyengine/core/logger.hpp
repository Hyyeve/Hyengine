#pragma once
#include <chrono>
#include <iomanip>
#include "../common/common.hpp"

#include "../library/glm.hpp"
#include "../common/sized_numerics.hpp"

#define MAKE_PRINTABLE(type, func) inline std::ostream& operator << (std::ostream& lhs, const type& rhs) { \
lhs << func; \
return lhs; }

MAKE_PRINTABLE(glm::vec2, '[' << rhs.x << ", " << rhs.y << ']');
MAKE_PRINTABLE(glm::ivec2, '[' << rhs.x << ", " << rhs.y << ']');
MAKE_PRINTABLE(glm::dvec2, '[' << rhs.x << ", " << rhs.y << ']');
MAKE_PRINTABLE(glm::vec3, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ']');
MAKE_PRINTABLE(glm::ivec3, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ']');
MAKE_PRINTABLE(glm::dvec3, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ']');
MAKE_PRINTABLE(glm::vec4, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ", " << rhs.w << ']');
MAKE_PRINTABLE(glm::ivec4, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ", " << rhs.w << ']');
MAKE_PRINTABLE(glm::dvec4, '[' << rhs.x << ", " << rhs.y << ", " << rhs.z << ", " << rhs.w << ']');

#undef MAKE_PRINTABLE

namespace hyengine
{
    enum class log_level : u8
    {
        ALL     = 3,
        NORMAL  = 2,
        REDUCED = 1,
        NONE    = 0
    };

    namespace ansi_codes
    {
        constexpr std::string_view ANSI_RESET = "\u001B[0m";
        constexpr std::string_view ANSI_BOLD = "\u001B[1m";
        constexpr std::string_view ANSI_UNDERLINE = "\u001B[4m";
        constexpr std::string_view ANSI_BLACK = "\u001B[30m";
        constexpr std::string_view ANSI_RED = "\u001B[31m";
        constexpr std::string_view ANSI_BRIGHT_RED = "\u001B[91m";
        constexpr std::string_view ANSI_GREEN = "\u001B[32m";
        constexpr std::string_view ANSI_BRIGHT_GREEN = "\u001B[92m";
        constexpr std::string_view ANSI_YELLOW = "\u001B[33m";
        constexpr std::string_view ANSI_BRIGHT_YELLOW = "\u001B[93m";
        constexpr std::string_view ANSI_BLUE = "\u001B[34m";
        constexpr std::string_view ANSI_BRIGHT_BLUE = "\u001B[94m";
        constexpr std::string_view ANSI_PURPLE = "\u001B[35m";
        constexpr std::string_view ANSI_BRIGHT_PURPLE = "\u001B[95m";
        constexpr std::string_view ANSI_CYAN = "\u001B[36m";
        constexpr std::string_view ANSI_BRIGHT_CYAN = "\u001B[96m";
        constexpr std::string_view ANSI_GREY = "\u001B[37m";
        constexpr std::string_view ANSI_WHITE = "\u001B[97m";
        constexpr std::string_view ANSI_DELETE_LINE = "\u001B[1A\u001B[2K\r";
    }

    void set_log_level(const log_level level);
    void flush_logs();
    void log(const std::string_view tag, const std::string_view msg, const std::string_view type, const std::string_view color_code);

    void log_debug(const std::string_view tag, const std::string_view msg);
    void log_info(const std::string_view tag, const std::string_view msg);
    void log_performance(const std::string_view tag, const std::string_view msg);
    void log_warn(const std::string_view tag, const std::string_view msg);
    void log_error(const std::string_view tag, const std::string_view msg);
    void log_fatal(const std::string_view tag, const std::string_view msg);
    void log_secret(const std::string_view tag, const std::string_view msg);


    #define VARARG_DEF(type) void type(const std::string_view id, const std::string_view first, const auto... rest) { type(id, stringify(first, rest...)); }

    VARARG_DEF(log_debug)
    VARARG_DEF(log_info)
    VARARG_DEF(log_performance)
    VARARG_DEF(log_warn)
    VARARG_DEF(log_error)
    VARARG_DEF(log_fatal)
    VARARG_DEF(log_secret)

    #undef VARARG_DEF
}
