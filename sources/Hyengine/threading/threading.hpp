#pragma once
#include <atomic>

namespace hyengine
{
    extern unsigned int IDLE_SLEEP_INCREMENT;

    class async_task
    {
    public:
        virtual ~async_task() = default;
        virtual void execute() = 0;

        void enqueue(unsigned int depends_on_id = 0);
        bool await(unsigned long timeout);
        bool finished();
        void run_now();
        unsigned int id();
        unsigned int dependency();

    private:
        std::atomic_bool is_finished;
        std::atomic_bool is_running;
        std::atomic_uint task_id;
        std::atomic_uint depends_on;
    };

    void release_threads();
    void create_threads();
    unsigned int current_thread_id();
}
