// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef YVM_CONCURRENT_H
#define YVM_CONCURRENT_H
#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

class SpinLock {
public:
    SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock&& operator=(SpinLock&&) = delete;

    inline void lock() noexcept {
        while (flag.test_and_set(std::memory_order_acquire))
            ;
    }
    inline void unlock() noexcept { flag.clear(std::memory_order_release); }

private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

class ThreadPool {
public:
    ThreadPool() : done(false) {}

    virtual ~ThreadPool() noexcept;

public:
    virtual void initialize(int startThreadNum) noexcept;

    virtual void runPendingWork();

    template <typename Func>
    future<void> submit(Func task);

    virtual void finalize() { done = true; }

protected:
    atomic_bool done{};
    vector<thread> threads;
    queue<packaged_task<void()>> taskQueue;
    mutex taskQueueMtx;
};

template <typename Func>
future<void> ThreadPool::submit(Func task) {
    packaged_task<void()> pt(task);
    future<void> f = pt.get_future();
    lock_guard<mutex> lock(taskQueueMtx);
    taskQueue.push(std::move(pt));
    return f;
}

#endif
