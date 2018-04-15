#ifndef YVM_YRUNTIME_H
#define YVM_YRUNTIME_H

#include <stack>
#include <unordered_map>
#include <condition_variable>
#include "Type.h"
#include "Frame.h"


struct JType;
struct Frame;
class JavaHeap;
class MethodArea;
class GC;

struct RuntimeEnv {
    RuntimeEnv();
    ~RuntimeEnv();

    MethodArea* ma;
    JavaHeap* jheap;
    std::unordered_map<std::string, JType*(*)(RuntimeEnv* env)> nativeMethods;
    GC* gc;
};

extern RuntimeEnv yrt;
extern thread_local StackFrames frames;
#endif // !YVM_YRUNTIME_H
