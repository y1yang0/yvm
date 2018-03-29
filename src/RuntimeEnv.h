#ifndef YVM_YRUNTIME_H
#define YVM_YRUNTIME_H

#include <stack>
#include <map>
#include "Type.h"


struct JType;
struct Frame;
class JavaHeap;
class MethodArea;

struct RuntimeEnv {
    RuntimeEnv();
    ~RuntimeEnv();

    u4 eip;
    MethodArea* ma;
    JavaHeap* jheap;
    std::stack<Frame*> frames;
    std::map<std::string, JType*(*)(RuntimeEnv* env)> nativeMethods;
};

extern RuntimeEnv yrt;
#endif // !YVM_YRUNTIME_H
