#include "threading.hpp"

#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <list>
#include <tracy/Tracy.hpp>

#include "../core/logger.hpp"
#include "hyengine/common/colors.hpp"

namespace hyengine
{
    static constexpr std::string_view LOGGER_TAG = "Async Task";

    static atomic_u32 thread_id_counter = 0;
    static std::unordered_map<u32, u32> thread_id_map;
    static std::mutex thread_id_lock;

    static std::vector<std::thread> threads;

    static std::mutex threadpool_work_lock;
    static std::condition_variable threadpool_work_condition;

    static bool should_threads_exit;
    static std::vector<threadpool_task*> waiting_tasks; //Tasks that cannot be executed yet due to waiting on dependencies
    static std::list<threadpool_task*> ready_tasks;     //Tasks that are not waiting on dependencies and can be dequeued

    void thread_loop()
    {
        ZoneScopedC(0x00FF77);
        while (true)
        {
            const bool did_execute_task = execute_next_task();
            if (did_execute_task) continue; //work until no more tasks available

            //Exit / work conditions could be changed here; thread would block until changes written

            std::unique_lock lock(threadpool_work_lock); //block until we get the lock

            //Exit/work conditions can't be changed here - would block and wait for this thread to check them

            if (should_threads_exit) break; //check if we should exit
            if (ready_tasks.empty())        //check if a task has been added since we finished the last one to prevent deadlock
            {
                threadpool_work_condition.wait(lock); //Lock released here, exit/work conditions can be changed and thread will see notifications
            }

            //Lock is re-locked here by waking thread

            lock.unlock(); //Release the lock while we do work
        }
    }

    void update_waiting_tasks()
    {
        ZoneScoped;
        threadpool_work_lock.lock();
        erase_if(waiting_tasks, [](threadpool_task* task)
        {
            const bool is_ready = task->dependencies_completed();
            if (is_ready) ready_tasks.push_back(task);
            return is_ready;
        });
        threadpool_work_lock.unlock();
    }

    //True if a task was executed
    bool execute_next_task()
    {
        threadpool_task* next_task = nullptr;
        threadpool_work_lock.lock();
        if (!ready_tasks.empty())
        {
            next_task = ready_tasks.front();
            ready_tasks.pop_front();
        }
        threadpool_work_lock.unlock();

        if (next_task == nullptr)
        {
            return false;
        }

        next_task->try_execute_task();
        update_waiting_tasks();

        return true;
    }

    bool has_next_task()
    {
        bool result = true;
        threadpool_work_lock.lock();
        if (ready_tasks.empty()) result = false;
        threadpool_work_lock.unlock();
        return result;
    }

    void release_threadpool()
    {
        ZoneScoped;

        threadpool_work_lock.lock();
        should_threads_exit = true;
        threadpool_work_lock.unlock();

        threadpool_work_condition.notify_all();

        for (std::thread& thread : threads)
        {
            thread.join();
        }

        threads.clear();
    }


    void create_threadpool()
    {
        ZoneScoped;
        threadpool_work_lock.lock();

        if (!threads.empty())
        {
            return;
        }

        should_threads_exit = false;

        //We want one task thread per logical core, leaving two cores free for the main thread and OS
        //If there isn't any logical cores available (or the hardware concurrency hint is unavailable) we just make 3.
        const i32 available_logical_cores = std::thread::hardware_concurrency() - 2;
        const u32 thread_count = static_cast<u32>(std::max(available_logical_cores, 3));
        threads.reserve(thread_count);
        for (u32 i = 0; i < thread_count; ++i)
        {
            threads.emplace_back([i]
            {
                // Tracy requires memory passed to the profiler to be pinned and never unallocated.
                // ReSharper disable once CppDFAMemoryLeak
                char* thread_name = new char[16];
                snprintf(thread_name, 16, " Worker %i ", i);
                tracy::SetThreadName(thread_name);
                thread_loop();
            });
        }

        threadpool_work_lock.unlock();
    }

    u32 get_current_thread_id()
    {
        thread_id_lock.lock();
        u32 id = 0;
        const u32 hash = std::hash<std::thread::id> {}(std::this_thread::get_id());
        if (thread_id_map.contains(hash))
        {
            id = thread_id_map[hash];
        }
        else
        {
            id = thread_id_counter;
            thread_id_counter += 1;
            thread_id_map[hash] = id;
        }
        thread_id_lock.unlock();

        return id;
    }

    void threadpool_task::enqueue()
    {
        if (!state_ready()) return; //Task already running or completed

        threadpool_work_lock.lock();
        const bool ready = dependencies_completed();

        if (ready)
        {
            ready_tasks.push_back(this);
        }
        else
        {
            waiting_tasks.push_back(this);
        }

        threadpool_work_lock.unlock();
        threadpool_work_condition.notify_one();
    }

    bool threadpool_task::completed() const
    {
        return state == execution_state::COMPLETED;
    }

    void threadpool_task::await_completed()
    {
        ZoneScoped;
        const bool executed_immediately = try_execute_task();
        if (executed_immediately) return;
        completion_future.wait();
    }

    bool threadpool_task::await_timeout(const u32 timeout_ms) const
    {
        ZoneScoped;
        completion_future.wait_for(std::chrono::milliseconds(timeout_ms));
        return completed();
    }

    bool threadpool_task::try_execute_task()
    {
        ZoneScoped;
        if (dependencies_completed() && state_ready())
        {
            state = execution_state::RUNNING;
            execute();
            completion_promise.set_value();
            state = execution_state::COMPLETED;
            return true;
        }

        return false;
    }

    bool threadpool_task::dependencies_completed() const
    {
        for (const threadpool_task* dependent : depends_on)
        {
            if (!dependent->completed()) return false;
        }
        return true;
    }

    bool threadpool_task::state_ready() const
    {
        return state == execution_state::WAITING;
    }
}
