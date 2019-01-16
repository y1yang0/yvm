#include "../gc/GC.h"
#include "JavaHeap.hpp"
#include "MethodArea.h"
#include "RuntimeEnv.h"

RuntimeEnv yrt;

RuntimeEnv::RuntimeEnv():ma(nullptr) {
    jheap = new JavaHeap;
    gc = new ConcurrentGC;
}

RuntimeEnv::~RuntimeEnv() {
    delete ma;
    delete jheap;
}
