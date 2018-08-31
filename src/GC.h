#ifndef _YVM_GC_H
#define _YVM_GC_H

#include <memory>
#include <unordered_set>
#include "Concurrent.hpp"
#include "Option.h"
#include "RuntimeEnv.h"

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
    void gc(GCPolicy policy = GCPolicy::GC_MARK_AND_SWEEP);

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
                yrt.gc->notifyGC();
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