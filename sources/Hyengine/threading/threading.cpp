#include "threading.hpp"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <list>

#include "../core/logger.hpp"

namespace hyengine::threading {

    using namespace hyengine;

    static constexpr std::string_view logger_tag = "Async Task";
    unsigned int IDLE_SLEEP_INCREMENT = 35;

    static std::atomic<unsigned int> thread_id_counter = 0;
    static std::unordered_map<unsigned int, unsigned int> thread_id_map;
    static std::mutex thread_id_lock;

    static std::vector<std::thread> threads;
    static std::pmr::unordered_set<unsigned int> pending_task_ids;
    static std::list<async_task*> tasks;
    static std::mutex task_lock;
    static std::atomic_bool threads_active = false;


    unsigned int next_task_id()
    {
        static std::atomic_uint next_task_id = 0;
        next_task_id += 1;
        if (next_task_id == 0) next_task_id = 1; //Could overflow, but zero is our "invalid task" value
        return next_task_id;
    }

    async_task* next_task()
    {
        async_task* task = nullptr;
        task_lock.lock();

        if (!tasks.empty())
        {
            task = tasks.back();
            tasks.pop_back();

            const int max_rotations = tasks.size();
            int rotation_count = 0;
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

    void mark_finished(async_task* task)
    {
        task_lock.lock();
        pending_task_ids.erase(task->id());
        task_lock.unlock();
    }

    //We do actually return when the threads are freed, but we never want to call this and block the main thread indefinitely.
    void process_tasks()
    {
        while(threads_active) {

            async_task* task = next_task();

            if (task != nullptr)
            {
                task->run_now();
                mark_finished(task);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_SLEEP_INCREMENT));
        }
    }

    void release_threads()
    {
        threads_active = false;

        for (std::thread& thread : threads)
        {
            thread.join();
        }

        threads.clear();
    }

    void create_threads()
    {
        if (!threads.empty())
        {
            return;
        }

        threads_active = true;

        //1 thread per core, minus main thread. Or just make 4 if we don't know how many cores we have.
        const unsigned int thread_count = std::max(std::thread::hardware_concurrency(), 5u) - 1u;
        threads.reserve(thread_count);
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            threads.emplace_back([] {
                process_tasks();
            });
        }
    }


    unsigned int current_thread_id()
    {
        thread_id_lock.lock();
        unsigned int id = 0;
        unsigned int hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
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

    void queue_task(async_task* task)
    {
        task_lock.lock();
        tasks.push_front(task);
        pending_task_ids.insert(task->id());
        task_lock.unlock();
    }


    void async_task::enqueue(unsigned int depends_on_id)
    {
        if (is_running)
        {
            logger::message_debug("Can't queue; this task object is already running!", logger_tag);
            return;
        }

        is_finished = false;
        is_running = true;
        task_id = next_task_id();
        depends_on = depends_on_id;

        queue_task(this);
    }

    bool async_task::await(unsigned long timeout)
    {
        if (!is_running)
        {
            return false;
        }

        unsigned long slept_amount = 0;
        while (!finished() && slept_amount < timeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_SLEEP_INCREMENT));
            slept_amount += IDLE_SLEEP_INCREMENT;
        }

        return finished();
    }

    bool async_task::finished()
    {
        return is_finished;
    }

    void async_task::run_now()
    {
        is_running = true;
        execute();
        is_running = false;
        is_finished = true;
    }

    unsigned int async_task::id()
    {
        return task_id;
    }

    unsigned int async_task::dependency()
    {
        return depends_on;
    }
}
