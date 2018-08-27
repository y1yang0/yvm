#include "ObjectMonitor.h"

void ObjectMonitor::enter(std::thread::id tid) {
    std::unique_lock<std::mutex> lock(internalMtx);

    if (monitorCnt == 0) {
        monitorCnt = 1;
        owner = tid;
        entered = true;
    } else {
        if (tid == owner) {
            monitorCnt++;
            entered = true;
        } else {
            cv.wait(lock, [=] { return monitorCnt == 0; });
            entered = true;
        }
    }
}

void ObjectMonitor::exit() {
    std::unique_lock<std::mutex> lock(internalMtx);

    if (!entered) {
        throw std::runtime_error("illegal monitor state");
    }

    monitorCnt--;
    if (monitorCnt == 0) {
        cv.notify_one();
    }
}
