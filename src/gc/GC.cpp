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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "GC.h"

#include <atomic>

#include "../runtime/ClassSpace.h"
#include "../runtime/JavaClass.h"
#include "../runtime/JavaHeap.hpp"
#include "../runtime/JavaType.h"
#include "../vm/YVM.h"
#include "Concurrent.hpp"

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

void ConcurrentGC::gc(JavaFrame* frames, GCPolicy policy) {
    lock_guard<mutex> lock(overMemoryThresholdMtx);
    this->frames = frames;
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
        auto fields = runtime.heap->getFields(dynamic_cast<JObject*>(ref));
        for (size_t i = 0; i < fields.size(); i++) {
            mark(fields[i]);
        }
    } else if (typeid(*ref) == typeid(JArray)) {
        {
            lock_guard<SpinLock> lock(arrSpin);
            arrayBitmap.insert(dynamic_cast<JArray*>(ref)->offset);
        }
        auto items = runtime.heap->getElements(dynamic_cast<JArray*>(ref));

        for (size_t i = 0; i < items.first; i++) {
            mark(items.second[i]);
        }
    } else {
        SHOULD_NOT_REACH_HERE
    }
}

void ConcurrentGC::sweep() {
    future<void> objectFuture = gcThreadPool.submit([this]() -> void {
        for (auto pos = runtime.heap->objectContainer.data.begin();
             pos != runtime.heap->objectContainer.data.end();) {
            // If we can not find active object in object bitmap then clear it
            // Notice that here we don't need to lock objectBitmap since it must
            // be marked before sweeping
            if (objectBitmap.find(pos->first) == objectBitmap.cend()) {
                runtime.heap->objectContainer.data.erase(pos++);
            } else {
                ++pos;
            }
        }
    });

    future<void> arrayFuture = gcThreadPool.submit([this]() -> void {
        for (auto pos = runtime.heap->arrayContainer.data.begin();
             pos != runtime.heap->arrayContainer.data.end();) {
            // DITTO
            if (arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                for (size_t i = 0; i < pos->second.first; i++) {
                    delete pos->second.second[i];
                }
                delete[] pos->second.second;
                runtime.heap->arrayContainer.data.erase(pos++);
            } else {
                ++pos;
            }
        }
    });

    future<void> monitorFuture = gcThreadPool.submit([this]() -> void {
        // DITTO
        for (auto pos = runtime.heap->monitorContainer.data.begin();
             pos != runtime.heap->monitorContainer.data.end();) {
            if (objectBitmap.find(pos->first) == objectBitmap.cend() ||
                arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                runtime.heap->monitorContainer.data.erase(pos++);
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
    auto* temp = frames->top();
    while (temp != nullptr) {
        stackMarkFuture.push_back(gcThreadPool.submit([this, temp]() -> void {
            for (int i = 0; i < temp->maxStack; i++) {
                this->mark(temp->stackSlots[i]);
            }
        }));

        localMarkFuture.push_back(gcThreadPool.submit([this, temp]() -> void {
            for (int i = 0; i < temp->maxLocal; i++) {
                this->mark(temp->localSlots[i]);
            }
        }));
        temp = temp->next;
    }

    future<void> staticFieldsFuture = gcThreadPool.submit([this]() -> void {
        for (auto c : runtime.cs->classTable) {
            for_each(c.second->staticVars.cbegin(), c.second->staticVars.cend(),
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

    for (auto& sk : stackMarkFuture) {
        sk.get();
    }
    for (auto& lv : localMarkFuture) {
        lv.get();
    }

    sweep();
}
