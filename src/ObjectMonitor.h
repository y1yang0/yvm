#ifndef YVM_OBJECTMONITOR_H
#define YVM_OBJECTMONITOR_H

#include <condition_variable>
#include <queue>
#include <thread>

class ObjectMonitor {
public:
	void enter(std::thread::id tid);
	void exit();

private:
	std::mutex internalMtx;

	volatile bool entered = false;
	int32_t monitorCnt    = 0;
	std::thread::id owner;
	std::condition_variable cv;
};

#endif
