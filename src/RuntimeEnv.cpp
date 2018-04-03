#include "RuntimeEnv.h"
#include "MethodArea.h"
#include "JavaHeap.h"
#include "Frame.h"

RuntimeEnv yrt;
thread_local StackFrames frames;

RuntimeEnv::RuntimeEnv(): aliveThreadCount(0) {
    ma = nullptr;
    jheap = new JavaHeap;
}

RuntimeEnv::~RuntimeEnv() {
    delete ma;
    delete jheap;
}
