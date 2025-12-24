#include "logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <tracy/Tracy.hpp>

#include "../threading/threading.hpp"
#include "hyengine/common/common.hpp"

namespace hyengine
{
    using namespace hyengine;

    class logging_flush_task final : public threadpool_task
    {
    public:
        std::vector<std::string> data;

        void execute() override
        {
            ZoneScopedN("Flush logs task");
            constexpr std::string_view time_format = std::string_view("\u001B[36m\u001B[1m");
            const std::time_t current_time = std::time(nullptr);

            // ReSharper disable once CppDeprecatedEntity
            const auto& now = std::localtime(&current_time);

            const std::string timestamp = stringify('[', time_format, now->tm_hour, ":", now->tm_min, ":", now->tm_sec, ansi_codes::ANSI_RESET, "]");
            for (const std::string& message : data)
            {
                if (message != ansi_codes::ANSI_DELETE_LINE) std::cout << timestamp;
                std::cout << message;
            }
        }
    };

    static std::mutex logging_lock;
    static log_level logging_level = log_level::NORMAL;

    static i32 log_repeat_count = 0;
    static std::string last_message;
    static std::string last_tag;
    static std::stringstream builder;
    static std::vector<std::string> message_queue;
    static bool has_buffered_messages = false;

    static logging_flush_task* current_flush_task = nullptr;

    void set_log_level(const log_level level)
    {
        logging_lock.lock();
        logging_level = level;
        logging_lock.unlock();
    }

    void flush_logs()
    {
        ZoneScoped;
        logging_lock.lock();

        if (!has_buffered_messages)
        {
            logging_lock.unlock();
            return;
        }

        if (current_flush_task != nullptr && !current_flush_task->completed())
        {
            logging_lock.unlock();
            return;
        }

        delete current_flush_task;
        current_flush_task = nullptr;

        current_flush_task = new logging_flush_task();
        current_flush_task->data = message_queue;
        current_flush_task->enqueue();

        message_queue.clear();
        has_buffered_messages = false;

        logging_lock.unlock();
    }

    inline void write_tag(const std::string_view format, const std::string_view color_code, const std::string_view tag)
    {
        builder << '[' << format << color_code << tag << ansi_codes::ANSI_RESET << ']';
    }

    inline void write_repeat_tag(const std::string_view color_code)
    {
        builder << ansi_codes::ANSI_RESET << " [" << ansi_codes::ANSI_BOLD << color_code << "+" << std::to_string(log_repeat_count) << ansi_codes::ANSI_RESET << ']';
    }

    void log(const std::string_view tag, const std::string_view msg, const std::string_view type, const std::string_view color_code)
    {
        ZoneScoped;

        logging_lock.lock();

        const bool is_repeat = msg == last_message && tag == last_tag;

        if (is_repeat)
        {
            log_repeat_count++;
            message_queue.emplace_back(ansi_codes::ANSI_DELETE_LINE);
        }
        else
        {
            log_repeat_count = 0;
            last_message = msg;
            last_tag = tag;
        }

        builder << stringify("[", ansi_codes::ANSI_BOLD, ansi_codes::ANSI_BRIGHT_BLUE, "T", std::setfill('0'), std::setw(3), get_current_thread_id(), ansi_codes::ANSI_RESET, ']');

        if (!type.empty()) write_tag(ansi_codes::ANSI_BOLD, color_code, type);
        if (!tag.empty()) write_tag(ansi_codes::ANSI_RESET, ansi_codes::ANSI_PURPLE, tag);

        builder << ' ' << color_code << msg << ansi_codes::ANSI_RESET;

        if (is_repeat) write_repeat_tag(color_code);

        builder << std::endl;

        message_queue.push_back(builder.str());
        builder.str("");

        has_buffered_messages = true;

        logging_lock.unlock();
    }

    void log_debug(const std::string_view tag, const std::string_view msg)
    {
        if (logging_level > log_level::NORMAL) log(tag, msg, "DEBG", ansi_codes::ANSI_GREEN);
    }

    void log_info(const std::string_view tag, const std::string_view msg)
    {
        if (logging_level > log_level::REDUCED) log(tag, msg, "INFO", ansi_codes::ANSI_BLUE);
    }

    void log_performance(const std::string_view tag, const std::string_view msg)
    {
        if (logging_level > log_level::REDUCED) log(tag, msg, "PERF", ansi_codes::ANSI_CYAN);
    }

    void log_warn(const std::string_view tag, const std::string_view msg)
    {
        if (logging_level > log_level::NONE) log(tag, msg, "WARN", ansi_codes::ANSI_BRIGHT_YELLOW);
    }

    void log_error(const std::string_view tag, const std::string_view msg)
    {
        if (logging_level > log_level::NONE) log(tag, msg, "ERRR", ansi_codes::ANSI_RED);
    }

    void log_fatal(const std::string_view tag, const std::string_view msg)
    {
        constexpr std::string_view fatal_format = std::string_view("\u001B[91m\u001B[1m\u001B[4m");
        log(tag, msg, "FATAL", fatal_format);
    }

    void log_secret(const std::string_view tag, const std::string_view msg)
    {
        log(tag, msg, "SECRET", ansi_codes::ANSI_BRIGHT_YELLOW);
    }
}
