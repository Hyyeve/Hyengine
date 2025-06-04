#include "logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>

#include "../threading/threading.hpp"

namespace hyengine::logger {
    using namespace hyengine;

    static std::mutex logging_lock;
    static log_level logging_level = log_level::NORMAL;
    static std::string last_log;
    static int log_repeat_count = 0;
    static std::stringstream message_buffer;

    std::string format_duration(const std::chrono::microseconds duration) {

        const auto us = duration;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        const auto s = std::chrono::duration_cast<std::chrono::seconds>(duration);

        std::stringstream format_stream;
        format_stream << std::fixed << std::setprecision(2);

        if(us.count() < 1000) {
            return std::to_string(us.count()) + "us";
        }

        if(ms.count() < 1000) {
            format_stream << static_cast<double>(ms.count()) + static_cast<double>(us.count() % 1000) / 1000.0;
            const std::string ms_fractional = format_stream.str();
            return ms_fractional + "ms";
        }

        format_stream << static_cast<double>(s.count()) + static_cast<double>(ms.count() % 1000) / 1000.0;
        std::string ms_fractional = format_stream.str();
        return ms_fractional + "s";
    }

    std::string format_secs(const double seconds) {
        return format_millis(seconds * 1000);
    }

    std::string format_millis(const double millis) {
        return format_duration(std::chrono::microseconds {static_cast<long>(millis * 1000)});
    }

    std::string format_bytes(const unsigned long bytes)
    {
        std::stringstream result;
        result << std::fixed << std::setprecision(2);
        double bytes_fractional = bytes;
        double bytes_scaled;
        if(bytes < 1e3) bytes_scaled = bytes;
        else if(bytes < 1e6) bytes_scaled = bytes_fractional / 1e3;
        else if(bytes < 1e9) bytes_scaled = bytes_fractional / 1e6;
        else bytes_scaled = bytes_fractional / 1e9;
        result << bytes_scaled;

        if(bytes < 1e3) result << " bytes";
        else if(bytes < 1e6) result << " kb";
        else if(bytes < 1e9) result << " mb";
        else result << " gb";

        return result.str();
    }

    std::string format_count(const unsigned long count_num, std::string_view count_of)
    {
        std::stringstream result;
        result << count_num << " " << count_of;
        if (count_num != 1) result << "s";
        return result.str();
    }


    void set_log_level(const log_level level)
    {
        logging_level = level;
    }

    void output_timestamp()
    {
        constexpr std::string_view TIME_FORMAT = std::string_view("\u001B[36m\u001B[1m");
        const std::time_t t = std::time(nullptr);
        std::cout << '[' << TIME_FORMAT;
        std::cout << std::put_time(std::localtime(&t), "%T") << ANSI_RESET << ']';
        std::cout << '[' << ANSI_BOLD << ANSI_BRIGHT_BLUE << "T" << std::setfill('0') << std::setw(3) << threading::current_thread_id() << ANSI_RESET << ']';
    }

    void message(const std::string_view type, const std::string_view msg, const std::string_view color_code, const std::string_view id) {

        logging_lock.lock();

        message_buffer.str({});
        if (!type.empty()) message_buffer << '[' << ANSI_BOLD << color_code << type << ANSI_RESET << ']';
        if (!id.empty()) message_buffer << '[' << ANSI_PURPLE << id << ANSI_RESET << ']';
        message_buffer << ' ' << color_code << msg << ANSI_RESET;

        const std::string formatted = message_buffer.str();
        if (formatted == last_log)
        {
            log_repeat_count++;
            std::cout << ANSI_DELETE_LINE;
            output_timestamp();
            std::cout << formatted << ANSI_RESET << " [" << ANSI_BOLD << color_code << "+" << std::to_string(log_repeat_count) << ANSI_RESET << ']';
        }
        else
        {
            log_repeat_count = 0;
            last_log = formatted;
            output_timestamp();
            std::cout << formatted;
        }

        std::cout << '\n';

        logging_lock.unlock();
    }

    void message_debug(const std::string_view msg, const std::string_view id) {
        if (logging_level > log_level::NORMAL) message("DEBG", msg, ANSI_GREEN, id);
    }

    void message_info(const std::string_view msg, const std::string_view id) {
        if (logging_level > log_level::REDUCED) message("INFO", msg, ANSI_BLUE, id);
    }

    void message_performance(const std::string_view msg, const std::string_view id) {
        if (logging_level > log_level::REDUCED) message("PERF", msg, ANSI_CYAN, id);
    }

    void message_warn(const std::string_view msg, const std::string_view id) {
        if (logging_level > log_level::NONE) message("WARN", msg, ANSI_BRIGHT_YELLOW, id);
    }

    void message_error(const std::string_view msg, const std::string_view id) {
        if (logging_level > log_level::NONE) message("ERRR", msg, ANSI_RED, id);
    }

    void message_fatal(const std::string_view msg, const std::string_view id) {
        constexpr std::string_view FATAL_FORMAT = std::string_view("\u001B[91m\u001B[1m\u001B[4m");
        message("FATL", msg, FATAL_FORMAT, id);
    }

    void message_secret(const std::string_view msg, const std::string_view id) {
        message("SECRET", msg, ANSI_BRIGHT_YELLOW, id);
    }
}
