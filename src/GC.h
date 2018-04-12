#ifndef _YVM_GC_H
#define _YVM_GC_H

#include "JavaHeap.h"
#include <unordered_set>

enum class GCPolicy {
    GC_MARK_AND_SWEEP
};

class GC{
public:
    GC(){}

    void gc(GCPolicy policy = GCPolicy::GC_MARK_AND_SWEEP);

private:
    void markAndSweep();
    void mark(JType* ref);
    void sweep() const;
    unordered_set<size_t> objectBitmap;
    unordered_set<size_t> arrayBitmap;
};

#endif