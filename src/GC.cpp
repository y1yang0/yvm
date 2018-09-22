#include <atomic>
#include "Concurrent.hpp"
#include "GC.h"
#include "JavaClass.h"
#include "JavaHeap.hpp"
#include "JavaType.h"
#include "MethodArea.h"
#include "YVM.h"

using namespace std;

void ConcurrentGC::stopTheWorld() {
    unique_lock<mutex> lock(safepointWaitMtx);
    safepointWaitCnt++;
    while (safepointWaitCnt != YVM::executor.getThreadNum()) {
        safepointWaitCond.wait(lock);
    }
    safepointWaitCond.notify_all();
}

void ConcurrentGC::GCThreadPool::finalize() {
    work = true;
    done = true;
    sleepCnd.notify_all();
}

void ConcurrentGC::GCThreadPool::runPendingWork() {
    while (!done) {
        unique_lock<mutex> lock(sleepMtx);
        while (work != true) {
            sleepCnd.wait(lock);
        }

        taskQueueMtx.lock();
        if (!taskQueue.empty()) {
            auto task = move(taskQueue.front());
            taskQueue.pop();
            taskQueueMtx.unlock();
            task();
        } else {
            taskQueueMtx.unlock();
        }
    }
}

void ConcurrentGC::gc(GCPolicy policy) {
    lock_guard<mutex> lock(overMemoryThresholdMtx);
    if (!overMemoryThreshold) {
        return;
    }

    switch (policy) {
        case GCPolicy::GC_MARK_AND_SWEEP:
            gcThreadPool.signalWork();
            markAndSweep();
            break;
        default:
            gcThreadPool.signalWork();
            markAndSweep();
            break;
    }
    objectBitmap.clear();
    arrayBitmap.clear();
    overMemoryThreshold = false;
    gcThreadPool.signalWait();
}

void ConcurrentGC::mark(JType* ref) {
    if (ref == nullptr) {
        // Prevent from throwing bad_typeid since local variable table slot
        // could be empty
        return;
    }
    if (typeid(*ref) == typeid(JObject)) {
        {
            // Mark() is very quickly and busy, so we use lightweight spin lock
            // instead of stl mutex
            lock_guard<SpinLock> lock(objSpin);
            objectBitmap.insert(dynamic_cast<JObject*>(ref)->offset);
        }
        auto fields = yrt.jheap->getFields(dynamic_cast<JObject*>(ref));
        for (size_t i = 0; i < fields.size(); i++) {
            mark(fields[i]);
        }
    } else if (typeid(*ref) == typeid(JArray)) {
        {
            lock_guard<SpinLock> lock(arrSpin);
            arrayBitmap.insert(dynamic_cast<JArray*>(ref)->offset);
        }
        auto items = yrt.jheap->getElements(dynamic_cast<JArray*>(ref));

        for (size_t i = 0; i < items.first; i++) {
            mark(items.second[i]);
        }
    } else {
        SHOULD_NOT_REACH_HERE
    }
}

void ConcurrentGC::sweep() {
    future<void> objectFuture = gcThreadPool.submit([this]() -> void {
        for (auto pos = yrt.jheap->objectContainer.data.begin();
             pos != yrt.jheap->objectContainer.data.end();) {
            // If we can not find active object in object bitmap then clear it
            // Notice that here we don't need to lock objectBitmap since it must
            // be marked before sweeping
            if (objectBitmap.find(pos->first) == objectBitmap.cend()) {
                yrt.jheap->objectContainer.data.erase(pos++);
            } else {
                ++pos;
            }
        }
    });

    future<void> arrayFuture = gcThreadPool.submit([this]() -> void {
        for (auto pos = yrt.jheap->arrayContainer.data.begin();
             pos != yrt.jheap->arrayContainer.data.end();) {
            // DITTO
            if (arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                for (size_t i = 0; i < pos->second.first; i++) {
                    delete pos->second.second[i];
                }
                delete[] pos->second.second;
                yrt.jheap->arrayContainer.data.erase(pos++);
            } else {
                ++pos;
            }
        }
    });

    future<void> monitorFuture = gcThreadPool.submit([this]() -> void {
        // DITTO
        for (auto pos = yrt.jheap->monitorContainer.data.begin();
             pos != yrt.jheap->monitorContainer.data.end();) {
            if (objectBitmap.find(pos->first) == objectBitmap.cend() ||
                arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                yrt.jheap->monitorContainer.data.erase(pos++);
            } else {
                ++pos;
            }
        }
    });

    objectFuture.get();
    arrayFuture.get();
    monitorFuture.get();
}

void ConcurrentGC::markAndSweep() {
    vector<future<void>> stackMarkFuture, localMarkFuture;
    for (auto frame = frames.cbegin(); frame != frames.cend(); ++frame) {
        stackMarkFuture.push_back(gcThreadPool.submit([this, frame]() -> void {
            for (auto stackSlot = (*frame)->stack.cbegin();
                 stackSlot != (*frame)->stack.cend(); ++stackSlot) {
                this->mark(*stackSlot);
            }
        }));

        localMarkFuture.push_back(gcThreadPool.submit([this, frame]() -> void {
            for (auto localSlot = (*frame)->locals.cbegin();
                 localSlot != (*frame)->locals.cend(); ++localSlot) {
                this->mark(*localSlot);
            }
        }));
    }

    future<void> staticFieldsFuture = gcThreadPool.submit([this]() -> void {
        for (auto c : yrt.ma->classTable) {
            for_each(
                c.second->sfield.cbegin(), c.second->sfield.cend(),
                [this](const pair<size_t, JType*>& offset) {
                    if (typeid(*offset.second) == typeid(JObject)) {
                        {
                            lock_guard<SpinLock> lock(objSpin);
                            objectBitmap.insert(offset.first);
                        }
                    } else if (typeid(*offset.second) == typeid(JArray)) {
                        {
                            lock_guard<SpinLock> lock(arrSpin);
                            arrayBitmap.insert(offset.first);
                        }
                    }
                });
        }
    });

    staticFieldsFuture.get();
    if (!frames.empty()) {
        for (auto& sk : stackMarkFuture) {
            sk.get();
        }
        for (auto& lv : localMarkFuture) {
            lv.get();
        }
    }
    sweep();
}
