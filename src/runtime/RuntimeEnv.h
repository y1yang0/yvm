#ifndef YVM_YRUNTIME_H
#define YVM_YRUNTIME_H

#include <unordered_map>

struct JType;
class JavaFrame;
class JavaHeap;
class MethodArea;
class ConcurrentGC;

struct RuntimeEnv {
    RuntimeEnv();
    ~RuntimeEnv();

    MethodArea* ma;
    JavaHeap* jheap;
    std::unordered_map<std::string, JType* (*)(RuntimeEnv* env, JType**,int)>
        nativeMethods;
    ConcurrentGC* gc;
};

extern RuntimeEnv yrt;
#endif  // !YVM_YRUNTIME_H
