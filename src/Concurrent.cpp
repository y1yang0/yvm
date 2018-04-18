#include "Concurrent.hpp"
#include "Option.h"
#include <iostream>
using namespace std;

void ThreadPool::initialize(int startThreadNum) noexcept{
    for(unsigned i=0;i<startThreadNum;i++) {
        threads.emplace_back(&ThreadPool::runPendingWork,this);
    }
}

ThreadPool::~ThreadPool() noexcept {
    done = true;
    for(thread &td : threads) {
        td.join();
    }
}

void ThreadPool::runPendingWork() {
    while(!done) {
        taskQueueMtx.lock();
        if (!taskQueue.empty()) {
            auto task = std::move(taskQueue.front());
            taskQueue.pop();
            taskQueueMtx.unlock();
            task();
        }
        else {
            taskQueueMtx.unlock();
            std::this_thread::yield();
        }
    }
}
