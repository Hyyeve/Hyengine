#pragma once
#include <chrono>

namespace hyengine::profiler {
    using namespace hyengine;

    struct section_time {
        std::chrono::microseconds total;
        std::chrono::microseconds sub;
        bool has_sub;
    };

    void start_timer(const std::string& id);
    unsigned long long end_timer(const std::string& id, const bool log_now);

    void start_section(const std::string& id);
    void end_section(const bool log_now = false, const bool record_section = true);
    void next_section(const std::string& id, const bool log_now = false, const bool record_section = true);

    [[nodiscard]] std::vector<std::string> get_recorded_section_ids();
    [[nodiscard]] section_time get_average(const std::string& id);

    void log_average(const std::string& id);
    void log_average();
    void log_all_sections();

    void clear_samples(const std::string& id);

}
