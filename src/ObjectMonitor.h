#ifndef YVM_OBJECTMONITOR_H
#define YVM_OBJECTMONITOR_H

#include <queue>
#include <thread>
#include <condition_variable>

class ObjectMonitor{
public:
    void enter(std::thread::id tid);
    void exit();

private:
    std::mutex internalMtx;
    
    volatile bool entered = false;
    int32_t monitorCnt = 0;
    std::thread::id owner;
    std::condition_variable cv;
};


#endif