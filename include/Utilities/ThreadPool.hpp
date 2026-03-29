#ifndef SOUNDBLAST_THREADPOOL_HPP
#define SOUNDBLAST_THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

namespace ShoutBlast
{
    class ThreadPool
    {
    public:
        ThreadPool(size_t threads);
        ~ThreadPool();
        void Enqueue(std::function<void()> task);
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
    };
}

#endif