#include "Frame.h"
#include "GC.h"
#include "JavaHeap.hpp"
#include "MethodArea.h"
#include "RuntimeEnv.h"

RuntimeEnv yrt;
thread_local StackFrames frames;

RuntimeEnv::RuntimeEnv() {
    ma = nullptr;
    jheap = new JavaHeap;
    gc = new ConcurrentGC;
}

RuntimeEnv::~RuntimeEnv() {
    delete ma;
    delete jheap;
}
