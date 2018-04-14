#ifndef YVM_JAVAFRAME_H
#define YVM_JAVAFRAME_H

#include "JavaType.h"
#include <deque>
#include <mutex>
#include <atomic>

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

class SpinLock {
public:
    SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock&& operator=(SpinLock&&) = delete;

    inline void lock()noexcept { while (flag.test_and_set(std::memory_order_acquire)); }
    inline void unlock()noexcept { flag.clear(std::memory_order_release); }
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
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
