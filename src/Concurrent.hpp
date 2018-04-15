#ifndef YVM_CONCURRENT_H
#define YVM_CONCURRENT_H
#include <queue>
#include <future>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>


using namespace std;

class SpinLock {
public:
    SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock&& operator=(SpinLock&&) = delete;

    inline void lock()noexcept { while (flag.test_and_set(std::memory_order_acquire)); }
    inline void unlock()noexcept { flag.clear(std::memory_order_release); }
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

class ThreadPool{
public:
    ThreadPool() noexcept;
    ~ThreadPool() noexcept;

    template<typename Func>
    future<void> submit(Func task);
    void storeTaskFuture(shared_future<void> taskFuture) { taskFutures.push_back(taskFuture); }
    vector<shared_future<void>> getTaskFutures() const { return taskFutures; }

    void suspendThreads() noexcept { stop = true; }
    void resumeThreads() noexcept { stop = false; stopCond.notify_all(); }

    void finalize() { done = true; }

private:
    void runPendingWork();
    void signalStop();

    unsigned defaultThreadCnt;
    vector<shared_future<void>> taskFutures;
    vector<thread> threads;
    queue<packaged_task<void()>> taskQueue;
    mutex taskQueueMtx;
    atomic_bool done{};
    atomic_bool stop{};

    mutex stopMtx;
    
    condition_variable stopCond;
};

template<typename Func>
future<void> ThreadPool::submit(Func task) {
    packaged_task<void()> pt(task);
    future<void> f = pt.get_future();
    lock_guard<mutex> lock(taskQueueMtx);
    taskQueue.push(std::move(pt));
    return f;
}

#endif

