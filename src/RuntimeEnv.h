#ifndef YVM_YRUNTIME_H
#define YVM_YRUNTIME_H

#include <stack>
#include <map>
#include <condition_variable>
#include "Type.h"
#include "Frame.h"


struct JType;
struct Frame;
class JavaHeap;
class MethodArea;

struct RuntimeEnv {
    RuntimeEnv();
    ~RuntimeEnv();

    MethodArea* ma;
    JavaHeap* jheap;
    std::map<std::string, JType*(*)(RuntimeEnv* env)> nativeMethods;

    std::mutex aliveThreadCounterMutex;
    int32_t aliveThreadCount;
    std::condition_variable noSubThreadCndVar;
};

extern RuntimeEnv yrt;
extern thread_local StackFrames frames;
#endif // !YVM_YRUNTIME_H
