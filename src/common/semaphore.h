#pragma once

#include <mutex>

class semaphore
{
public:
    explicit semaphore(int count) noexcept
        : count_(count) { assert(count > -1); }

    void post() noexcept
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++count_;
        }
        cv_.notify_one();
    }

    void wait() noexcept
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]() { return count_ != 0; });
        --count_;
    }

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


