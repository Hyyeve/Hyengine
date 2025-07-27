#include "threading.hpp"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <list>
#include <tracy/Tracy.hpp>

#include "../core/logger.hpp"

namespace hyengine
{
    static constexpr std::string_view logger_tag = "Async Task";
    u32 IDLE_SLEEP_INCREMENT = 35;

    static std::atomic<u32> thread_id_counter = 0;
    static std::unordered_map<u32, u32> thread_id_map;
    static std::mutex thread_id_lock;

    static std::vector<std::thread> threads;
    static std::pmr::unordered_set<u32> pending_task_ids;
    static std::list<threadpool_task*> tasks;
    static std::mutex task_lock;
    static std::atomic_bool threads_active = false;


    u32 next_task_id()
    {
        static atomic_u32 next_task_id = 0;
        next_task_id += 1;
        if (next_task_id == 0) next_task_id = 1; //Could overflow, but zero is our "invalid task" value
        return next_task_id;
    }

    threadpool_task* next_task()
    {
        threadpool_task* task = nullptr;
        task_lock.lock();

        if (!tasks.empty())
        {
            task = tasks.back();
            tasks.pop_back();

            const i32 max_rotations = tasks.size();
            i32 rotation_count = 0;
            while (pending_task_ids.contains(task->dependency()))
            {
                if (rotation_count >= max_rotations)
                {
                    //If we've gotten here, either we have a dependency loop, or everything is waiting on tasks that are currently executing.
                    //Put it back in the list and give up.
                    tasks.push_front(task);
                    task = nullptr;
                    break;
                }

                //Task we pulled is dependent on something that's pending - push it back into the queue and grab a new one
                tasks.push_front(task);
                task = tasks.back();
                tasks.pop_back();
                rotation_count++;
            }
        }

        task_lock.unlock();
        return task;
    }

    void mark_finished(threadpool_task* task)
    {
        task_lock.lock();
        pending_task_ids.erase(task->id());
        task_lock.unlock();
    }

    //We do actually return when the threads are freed, but we never want to call this and block the main thread indefinitely.
    void process_tasks()
    {
        ZoneScoped;
        while (threads_active)
        {
            threadpool_task* task = next_task();

            if (task != nullptr)
            {
                task->execute_blocking();
                mark_finished(task);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_SLEEP_INCREMENT));
        }
    }

    void release_threadpool()
    {
        threads_active = false;

        for (std::thread& thread : threads)
        {
            thread.join();
        }

        threads.clear();
    }

    void create_threadpool()
    {
        if (!threads.empty())
        {
            return;
        }

        threads_active = true;

        //1 thread per core, minus main thread. Or just make 4 if we don't know how many cores we have.
        const u32 thread_count = std::max(std::thread::hardware_concurrency(), 5u) - 1u;
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
                process_tasks();
            });
        }
    }


    u32 get_current_thread_id()
    {
        thread_id_lock.lock();
        u32 id = 0;
        const u32 hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
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

    void queue_task(threadpool_task* task)
    {
        task_lock.lock();
        tasks.push_front(task);
        pending_task_ids.insert(task->id());
        task_lock.unlock();
    }


    void threadpool_task::enqueue(const u32 depends_on_id)
    {
        if (is_running)
        {
            log_debug(logger_tag, "Can't queue; this task object is already running!");
            return;
        }

        is_finished = false;
        is_running = true;
        task_id = next_task_id();
        depends_on = depends_on_id;

        queue_task(this);
    }

    bool threadpool_task::await(const u64 timeout)
    {
        if (!is_running)
        {
            return false;
        }

        u64 slept_amount = 0;
        while (!finished() && slept_amount < timeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_SLEEP_INCREMENT));
            slept_amount += IDLE_SLEEP_INCREMENT;
        }

        return finished();
    }

    bool threadpool_task::finished()
    {
        return is_finished;
    }

    void threadpool_task::execute_blocking()
    {
        ZoneScoped;
        is_running = true;
        is_finished = false;
        execute();
        is_running = false;
        is_finished = true;
    }

    u32 threadpool_task::id()
    {
        return task_id;
    }

    u32 threadpool_task::dependency()
    {
        return depends_on;
    }
}
