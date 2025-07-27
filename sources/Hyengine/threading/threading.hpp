#pragma once
#include <atomic>

#include "Hyengine/common/sized_numerics.hpp"

namespace hyengine
{
    extern u32 IDLE_SLEEP_INCREMENT;

    class threadpool_task
    {
    public:
        virtual ~threadpool_task() = default;
        virtual void execute() = 0;

        void enqueue(u32 depends_on_id = 0);
        bool await(uint64_t timeout);
        bool finished();
        void execute_blocking();
        u32 id();
        u32 dependency();

    private:
        std::atomic_bool is_finished;
        std::atomic_bool is_running;
        std::atomic_uint task_id;
        std::atomic_uint depends_on;
    };

    void release_threadpool();
    void create_threadpool();
    u32 get_current_thread_id();
}
