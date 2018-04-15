#include "RuntimeEnv.h"
#include "MethodArea.h"
#include "JavaHeap.h"
#include "Frame.h"
#include "GC.h"

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
