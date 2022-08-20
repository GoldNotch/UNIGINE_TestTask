#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>


class ThreadPool
{
public:
    ThreadPool(size_t threads_count = std::thread::hardware_concurrency())
    {
        is_alive = true;
        threads.resize(threads_count);
        for(auto& th: threads)
            th = std::thread(ThreadPool::ThreadMain, this);
    }
    ~ThreadPool()
    {
        is_alive = false;
        mutex_condition.notify_all();
        for(auto& thread: threads)
            thread.join();
        threads.clear();
    }

    using TaskFunc = std::function<void()>;
    inline void QueueTask(const TaskFunc& task)
    {
        {
            std::lock_guard<std::mutex> lk(use_queue);
            task_queue.push(task);
        }
        mutex_condition.notify_one();
    }

private:
    std::vector<std::thread> threads;
    std::atomic<bool> is_alive;
    std::queue<TaskFunc> task_queue;
    std::mutex use_queue;
    std::condition_variable mutex_condition;

    void ThreadMain()
    {
        while(is_alive)
        {
            TaskFunc task;
            {
                std::unique_lock<std::mutex> lk(use_queue);
                mutex_condition.wait(lk, [this]{return !task_queue.empty() || !is_alive;});
                if (!is_alive) return;
                task = task_queue.front();
                task_queue.pop();
            }
            try{
                task();
            }catch (std::exception& e){
                printf("exception in task: %s - %s\n", e.what(), typeid(e).name());
                return;
            }
        }
    }
};
