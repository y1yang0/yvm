#include "RuntimeEnv.h"
#include "MethodArea.h"
#include "JavaHeap.h"
#include "Frame.h"
RuntimeEnv yrt;

RuntimeEnv::RuntimeEnv(): eip(0){
    ma = nullptr;
    jheap = new JavaHeap;
}

RuntimeEnv::~RuntimeEnv() {
    delete ma;
    delete jheap;
    while (!frames.empty()) {
        Frame * top = frames.top();
        frames.pop();
        delete top;
    }
}
