#ifndef YVM_JAVAFRAME_H
#define YVM_JAVAFRAME_H

#include "JavaType.h"
#include <deque>
#include <mutex>
#include <atomic>
#include "Concurrent.hpp"

using namespace std;

struct JType;

struct Frame {
    deque<JType*> locals;
    deque<JType*> stack;

    ~Frame() {
        while (!locals.empty()){
            auto* temp = locals.back();
            locals.pop_back();
            delete temp;
        }
        while (!stack.empty()) {
            auto* type = stack.back();
            stack.pop_back();
            delete type;
        }
    }
};

class StackFrames {
public:
    StackFrames() = default;
    ~StackFrames() {
        lock_guard<SpinLock> lock(spinMtx);
        while (!frames.empty()) {
            auto temp = frames.back();
            frames.pop_back();
            delete temp;
        }
    }

    inline auto back() noexcept { lock_guard<SpinLock> lock(spinMtx);return frames.back();}
    inline void push_back(Frame * f) { lock_guard<SpinLock> lock(spinMtx); frames.push_back(f); }
    inline void pop_back() { lock_guard<SpinLock> lock(spinMtx); frames.pop_back(); }
    inline bool empty() { lock_guard<SpinLock> lock(spinMtx); return frames.empty(); }
    inline auto cbegin() noexcept{ lock_guard<SpinLock> lock(spinMtx); return frames.cbegin(); }
    inline auto cend() noexcept { lock_guard<SpinLock> lock(spinMtx); return frames.cend(); }
private:
    deque<Frame*> frames;
    SpinLock spinMtx;
};

#endif
