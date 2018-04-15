#include "Concurrent.hpp"
#include "Option.h"
#include <iostream>
using namespace std;

ThreadPool::ThreadPool() noexcept :defaultThreadCnt(std::thread::hardware_concurrency()), done(false){
    for(unsigned i=0;i<defaultThreadCnt;i++) {
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
