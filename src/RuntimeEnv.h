#ifndef YVM_YRUNTIME_H
#define YVM_YRUNTIME_H

#include "Frame.h"
#include "Type.h"
#include <condition_variable>
#include <stack>
#include <unordered_map>

struct JType;
struct Frame;
class JavaHeap;
class MethodArea;
class ConcurrentGC;

struct RuntimeEnv {
	RuntimeEnv();
	~RuntimeEnv();

	MethodArea* ma;
	JavaHeap* jheap;
	std::unordered_map<std::string, JType* (*) (RuntimeEnv* env)> nativeMethods;
	ConcurrentGC* gc;
};

extern RuntimeEnv yrt;
extern thread_local StackFrames frames;
#endif // !YVM_YRUNTIME_H
