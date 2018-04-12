#include "RuntimeEnv.h"
#include "MethodArea.h"
#include "JavaHeap.h"
#include "Frame.h"
#include "GC.h"

RuntimeEnv yrt;
thread_local StackFrames frames;

RuntimeEnv::RuntimeEnv(): aliveThreadCount(0) {
    ma = nullptr;
    jheap = new JavaHeap;
    gc = new GC;
}

RuntimeEnv::~RuntimeEnv() {
    delete ma;
    delete jheap;
}
