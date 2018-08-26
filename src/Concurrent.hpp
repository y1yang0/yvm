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
	SpinLock()                = default;
	SpinLock(const SpinLock&) = delete;
	SpinLock& operator=(const SpinLock&) = delete;
	SpinLock(SpinLock&&)                 = delete;
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

	template <typename Func> future<void> submit(Func task);

	virtual void finalize() { done = true; }

protected:
	atomic_bool done{};
	vector<thread> threads;
	queue<packaged_task<void()>> taskQueue;
	mutex taskQueueMtx;
};

template <typename Func> future<void> ThreadPool::submit(Func task) {
	packaged_task<void()> pt(task);
	future<void> f = pt.get_future();
	lock_guard<mutex> lock(taskQueueMtx);
	taskQueue.push(std::move(pt));
	return f;
}

#endif
