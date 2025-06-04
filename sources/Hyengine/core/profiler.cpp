#include "profiler.hpp"

#include <chrono>
#include <mutex>
#include <ranges>
#include <stack>
#include <unordered_map>

#include "logger.hpp"
#include "../common/ring_buffer.hpp"
#include "../threading/threading.hpp"

namespace hyengine::profiler {
    using namespace hyengine;

    using time_duration = std::chrono::microseconds;
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using sample_buffer = common::ring_buffer<section_time, 100>;

    std::string logger_tag = "Profiler";

    struct section_timer {
        time_point start;
        time_duration sub_time;
        bool has_sub_time;
    };

    struct thread_local_data
    {
        std::stack<std::string> section_stack;
        std::stack<section_timer> timer_stack;
        std::unordered_map<std::string, sample_buffer> recorded_times;
        std::unordered_map<std::string, section_timer> standalone_timers;
    };

    std::unordered_map<unsigned int, thread_local_data> thread_datasets;


    thread_local_data& get_local_data()
    {
        unsigned int id = threading::current_thread_id();
        if (thread_datasets.contains(id))
        {
            return thread_datasets.at(id);
        }

        thread_local_data data = thread_local_data();
        thread_datasets.emplace(id, data);
        return thread_datasets.at(id);
    }

    time_point get_time() {
        return std::chrono::high_resolution_clock::now();
    }

    void log_sample(const std::string& base_message, const std::string& section_string, const section_time& sample) {
        const std::string time_string = logger::format_duration(sample.total);
        const std::string subtime_string = sample.has_sub ? " (" + logger::format_duration(sample.sub) + " in subsections)" : "";
        logger::performance(logger_tag, "[", section_string, "] ", base_message + time_string + subtime_string);
    }

    void record_sample(const std::string& section_string, const section_time& sample) {
        thread_local_data& data = get_local_data();

        const auto recorder = data.recorded_times.find(section_string);
        if(recorder != data.recorded_times.end()) {
            recorder->second.push(sample);
        }
        else {
            sample_buffer new_buffer;
            new_buffer.push(sample);
            data.recorded_times.insert(std::make_pair(section_string, new_buffer));
        }
    }

    void start_section(const std::string &id) {
        thread_local_data& data = get_local_data();

        const std::string current_section = data.section_stack.empty() ? id : data.section_stack.top() + " > " + id;
        data.section_stack.push(current_section);
        data.timer_stack.push({get_time()});
    }

    void end_section(const bool log_now, const bool record_section) {
        thread_local_data& data = get_local_data();
        const auto time_now = get_time();

        if(data.section_stack.empty()) {
            logger::warn(logger_tag, "Attempted to pop profiler section with no sections active!");
            return;
        }

        const std::string section_string = data.section_stack.top();
        const section_timer popped_timer = data.timer_stack.top();
        data.section_stack.pop();
        data.timer_stack.pop();

        const std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(time_now - popped_timer.start);

        if(!data.timer_stack.empty()) {
            section_timer& remaining_timer = data.timer_stack.top();
            remaining_timer.sub_time += duration;
            remaining_timer.has_sub_time = true;
        }

        if(record_section) {
            record_sample(section_string, {duration, popped_timer.sub_time, popped_timer.has_sub_time});
        }

        if(log_now) {
            log_sample("Section took ", section_string, {duration, popped_timer.sub_time, popped_timer.has_sub_time});
        }
    }

    void start_timer(const std::string& id)
    {
        thread_local_data& data = get_local_data();
        const auto time_now = get_time();
        data.standalone_timers[id].start = time_now;
    }

    unsigned long long end_timer(const std::string& id, const bool log_now)
    {
        thread_local_data& data = get_local_data();
        const auto time_now = get_time();

        if (data.standalone_timers.contains(id))
        {
            const std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(time_now - data.standalone_timers[id].start);
            data.standalone_timers.erase(id);

            if (log_now)
            {
                logger::performance(logger_tag, id, "Timer ran for ", logger::format_duration(duration));
            }

            return duration.count();
        }

        logger::warn(logger_tag, id, " Timer not found in timers for this thread!");
        return 0;
    }

    void next_section(const std::string& id, const bool log_now, const bool record_section) {
        end_section(log_now, record_section);
        start_section(id);
    }

    section_time get_average(const std::string& id) {
        thread_local_data& data = get_local_data();
        sample_buffer buff;
        bool got_sample = false;

        const auto recorder = data.recorded_times.find(id);

        if(recorder != data.recorded_times.end()) {
            got_sample = true;
            buff = recorder->second;
        }
        else {
            for (const auto& recorded_time: data.recorded_times) {
                if(recorded_time.first.ends_with(id)) {
                    buff = recorded_time.second;
                    got_sample = true;
                    break;
                }
            }
        }

        if(got_sample) {

            time_duration total = time_duration(0);
            time_duration total_sub = time_duration(0);
            bool has_sub = true;
            const int total_samples = buff.max_index() + 1;
            for(int i = 0; i < total_samples; i++) {
                const section_time time = buff[i];
                total += time.total;
                total_sub += time.sub;
                has_sub = has_sub && time.has_sub;
            }

            const time_duration average = total / total_samples;
            const time_duration sub_average = total_sub / total_samples;

            return {average, sub_average, has_sub};
        }

        logger::warn(logger_tag, "Couldn't find section \"" + id + "\" in recorded profiler sections");
        return {};
    }

    std::vector<std::string> get_recorded_section_ids() {
        thread_local_data& data = get_local_data();
        std::vector<std::string> ids;
        for(const auto& key: data.recorded_times | std::views::keys) {
            ids.push_back(key);
        }
        return ids;
    }

    void log_average(const std::string& id) {
        const section_time average = get_average(id);
        log_sample("Section averaged ", id, average);
    }

    void log_average() {
        thread_local_data& data = get_local_data();
        log_average(data.section_stack.top());
    }

    void log_all_sections() {
        for (const std::string& id: get_recorded_section_ids()) {
            log_average(id);
        }
    }

    void clear_samples(const std::string &id) {
        thread_local_data& data = get_local_data();
        data.recorded_times.erase(id);
    }
}
