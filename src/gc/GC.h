// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef _YVM_GC_H
#define _YVM_GC_H

#include <memory>
#include <unordered_set>
#include "Concurrent.hpp"
#include "../misc/Option.h"
#include "../runtime/RuntimeEnv.h"

using namespace std;

struct JType;

enum class GCPolicy { GC_MARK_AND_SWEEP };
class ConcurrentGC {
public:
    ConcurrentGC() : overMemoryThreshold(false), safepointWaitCnt(0) {
        gcThreadPool.initialize(thread::hardware_concurrency());
    }

    bool shallGC() const { return overMemoryThreshold; }
    void notifyGC() {
        safepointWaitCnt = 0;
        overMemoryThreshold = true;
    }
    void stopTheWorld();
    void gc(JavaFrame* frames, GCPolicy policy = GCPolicy::GC_MARK_AND_SWEEP);

    void terminateGC() { gcThreadPool.finalize(); }

private:
    inline void pushObjectBitmap(size_t offset) { objectBitmap.insert(offset); }

    void markAndSweep();
    void mark(JType* ref);
    void sweep();
    unordered_set<size_t> objectBitmap;
    SpinLock objSpin;

    unordered_set<size_t> arrayBitmap;
    SpinLock arrSpin;
    atomic_bool overMemoryThreshold;
    mutex overMemoryThresholdMtx;

    int safepointWaitCnt;
    mutex safepointWaitMtx;
    condition_variable safepointWaitCond;

private:
    struct GCThreadPool : ThreadPool {
        GCThreadPool() : ThreadPool(), work(false) {}

        void signalWork() {
            work = true;
            sleepCnd.notify_all();
        }
        void signalWait() { work = false; }

        void finalize() override;
        void runPendingWork() override;

        atomic_bool work;
        mutex sleepMtx;
        condition_variable sleepCnd;
    };
    JavaFrame* frames;
    GCThreadPool gcThreadPool;
};

class GC;
template <class T>
struct HeapAllocator {
    typedef T value_type;
    HeapAllocator() = default;
    HeapAllocator(const HeapAllocator& rhs) {
        this->thresholdVal = rhs.thresholdVal;
    }
    HeapAllocator& operator=(const HeapAllocator& rhs) {
        this->thresholdVal = rhs.thresholdVal;
        return *this;
    }

    template <class U>
    constexpr HeapAllocator(const HeapAllocator<U>&) noexcept {}
    T* allocate(size_t n) {
        if (n > size_t(-1) / sizeof(T)) throw bad_alloc();
        if (auto p = static_cast<T*>(malloc(n * sizeof(T)))) {
            thresholdVal += n * sizeof(T);
            if (thresholdVal >= YVM_GC_THRESHOLD_VALUE) {
                runtime.gc->notifyGC();
                thresholdVal = 0;
            }
            return p;
        }
        throw bad_alloc();
    }
    void deallocate(T* p, size_t val) noexcept {
        free(p);
        thresholdVal -= val;
    }

private:
    size_t thresholdVal = 0;
};

#endif
