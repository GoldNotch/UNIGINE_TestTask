#pragma once
#include <thread>
#include <atomic>
#include <queue>
#include <vector>
#include <mutex>

class ThreadPool
{
public:
    ThreadPool(size_t threads_count = std::thread::hardware_concurrency())
    {
        threads.resize(threads_count);
        for(auto& th: threads)
            th = std::thread(ThreadPool::ThreadMain, this);
    }
    ~ThreadPool()
    {
        is_alive = false;
        for(auto& thread: threads)
            thread.join();
        threads.clear();
    }

    using TaskFunc = std::function<void()>;
    void QueueTask(const TaskFunc& task)
    {
        use_queue.lock();
        task_queue.push(task);
        use_queue.unlock();
    }

private:
    std::vector<std::thread> threads;
    std::atomic<bool> is_alive;
    std::queue<TaskFunc> task_queue;
    std::mutex use_queue;

    void ThreadMain()
    {
        while(is_alive)
        {
            use_queue.lock();
            bool has_tasks = !task_queue.empty();
            use_queue.unlock();
            if (has_tasks)
            {

            }
        }
    }
};
