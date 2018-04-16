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
    ThreadPool() = delete;
    ThreadPool(int startThreadNum) noexcept;
    ~ThreadPool() noexcept;

    void createThread() { threads.emplace_back(&ThreadPool::runPendingWork, this); }

    size_t getThreadNum() const { return threads.size(); }

    template<typename Func>
    future<void> submit(Func task);
    
    void storeTaskFuture(shared_future<void> taskFuture) { taskFutures.push_back(taskFuture); }
    
    vector<shared_future<void>> getTaskFutures() const { return taskFutures; }


    void finalize() { done = true; }

private:
    void runPendingWork();
    atomic_bool done{};

    vector<shared_future<void>> taskFutures;
    vector<thread> threads;
    queue<packaged_task<void()>> taskQueue;
    mutex taskQueueMtx; 
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

