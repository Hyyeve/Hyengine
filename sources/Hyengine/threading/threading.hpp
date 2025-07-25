#pragma once
#include <atomic>

namespace hyengine
{
    extern unsigned int IDLE_SLEEP_INCREMENT;

    class threadpool_task
    {
    public:
        virtual ~threadpool_task() = default;
        virtual void execute() = 0;

        void enqueue(unsigned int depends_on_id = 0);
        bool await(unsigned long timeout);
        bool finished();
        void execute_blocking();
        unsigned int id();
        unsigned int dependency();

    private:
        std::atomic_bool is_finished;
        std::atomic_bool is_running;
        std::atomic_uint task_id;
        std::atomic_uint depends_on;
    };

    void release_threadpool();
    void create_threadpool();
    unsigned int get_current_thread_id();
}
