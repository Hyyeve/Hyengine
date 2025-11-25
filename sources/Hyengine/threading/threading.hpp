#pragma once
#include <atomic>
#include <condition_variable>
#include <future>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    void create_threadpool();
    void release_threadpool();

    bool execute_next_task();
    bool has_next_task();

    u32 get_current_thread_id();

    class threadpool_task
    {
    public:
        threadpool_task(const std::initializer_list<threadpool_task*> dependencies) : depends_on(dependencies)
        {
            completion_promise = std::promise<void>();
            completion_future = completion_promise.get_future();
        }

        threadpool_task() : threadpool_task({}) {}
        virtual ~threadpool_task() = default;

        void enqueue();
        bool completed() const;
        void await_completed();
        bool await_timeout(u32 timeout_ms) const;

    protected:
        virtual void execute() = 0;

    private:
        enum class execution_state : u8
        {
            WAITING, RUNNING, COMPLETED
        };

        //Attempts to execute the task, will fail and return false if the task is already in progress/complete or dependencies are not complete
        bool try_execute_task();

        //All dependencies are complete?
        bool dependencies_completed() const;

        //We're not already executing or complete?
        bool state_ready() const;

        std::atomic<execution_state> state = execution_state::WAITING;

        //Must be written ONCE on creation and never again
        std::vector<threadpool_task*> depends_on;

        std::promise<void> completion_promise;
        std::future<void> completion_future;

        friend bool execute_next_task();
        friend void update_waiting_tasks();
    };
}
