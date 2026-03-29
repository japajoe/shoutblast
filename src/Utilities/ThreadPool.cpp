#include "ThreadPool.hpp"

namespace ShoutBlast
{
    ThreadPool::ThreadPool(size_t threads) : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back([this]
            {
                while (true)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        
                        // Wait until the pool is stopped or there is a task to execute
                        this->condition.wait(lock, [this] 
                        { 
                            return this->stop || !this->tasks.empty(); 
                        });

                        if (this->stop && this->tasks.empty())
                        {
                            return;
                        }

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void ThreadPool::Enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        
        condition.notify_all();

        for (std::thread &worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
}