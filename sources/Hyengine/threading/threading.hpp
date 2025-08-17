#pragma once
#include <atomic>
#include <condition_variable>
#include <future>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    class threadpool_task
    {
    public:
        virtual ~threadpool_task() = default;
        virtual void execute() = 0;

        void enqueue(u32 depends_on_id = 0);
        bool await(uint64_t timeout) const;
        bool finished() const;
        void execute_blocking();
        u32 id();
        u32 dependency();

    private:
        std::atomic_bool is_finished;
        std::atomic_bool is_running;
        std::atomic_uint task_id;
        std::atomic_uint depends_on;
        std::promise<void> completion_promise;
        std::future<void> completion_future;
    };

    void release_threadpool();
    void create_threadpool();
    u32 get_current_thread_id();
}
