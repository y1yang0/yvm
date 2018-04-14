#include "GC.h"
#include "JavaType.h"
#include "MethodArea.h"
#include "JavaHeap.h"
#include "Concurrent.hpp"

void GC::gc(GCPolicy policy) {
    switch (policy) {
    case GCPolicy::GC_MARK_AND_SWEEP:
        markAndSweep();
        break;
    default:
        markAndSweep();
        break;
    } 
}

void GC::mark(JType* ref) {
    if (typeid(*ref) == typeid(JObject)) {
        objectBitmap.insert(dynamic_cast<JObject*>(ref)->offset);
        auto & fields = yrt.jheap->getObjectFieldsByOffset(dynamic_cast<JObject*>(ref)->offset);
        for(size_t i=0;i<fields.size();i++) {
            mark(fields[i]);
        }
    }else if(typeid(*ref) == typeid(JArray)) {
        arrayBitmap.insert(dynamic_cast<JArray*>(ref)->offset);
        auto & items = yrt.jheap->getArrayItemsByOffset(dynamic_cast<JArray*>(ref)->offset);

        for(size_t i=0;i<items.first;i++) {
            mark(items.second[i]);
        }
    }else {
        SHOULD_NOT_REACH_HERE
    }
}

void GC::sweep(){
    future<void> objectFuture = gcThreadPool.submit([this]()->void{
        lock_guard<recursive_mutex> lock(yrt.jheap->objMtx);
        for (auto pos = yrt.jheap->objheap.begin(); pos != yrt.jheap->objheap.end();) {
            // If we can not find active object in object bitmap then clear it
            // Notice that here we don't need to lock objectBitmap since it must 
            // be marked before sweeping
            if (objectBitmap.find(pos->first) == objectBitmap.cend()) {
                yrt.jheap->objheap.erase(pos++);
            }
            else {
                ++pos;
            }
        }
    });

    future<void> arrayFuture = gcThreadPool.submit([this]()->void {
        for (auto pos = yrt.jheap->arrheap.begin(); pos != yrt.jheap->arrheap.end();) {
            // DITTO
            if (arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                for (size_t i = 0; i < pos->second.first; i++) {
                    delete pos->second.second[i];
                }
                delete[] pos->second.second;
                yrt.jheap->arrheap.erase(pos++);
            }
            else {
                ++pos;
            }
        }
    });

    future<void> monitorFuture = gcThreadPool.submit([this]()->void {
        for (auto pos = yrt.jheap->monitorheap.begin(); pos != yrt.jheap->monitorheap.end();) {
            if (objectBitmap.find(pos->first) == objectBitmap.cend() || arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
                yrt.jheap->monitorheap.erase(pos++);
            }
            else {
                ++pos;
            }
        }
    });

    objectFuture.get();
    arrayFuture.get();
    monitorFuture.get();
}
void GC::markAndSweep() {  
    future<void> stackMarkFuture, localMarkFuture;
    for(auto frame = frames.cbegin();frame!=frames.cend();++frame) {
        stackMarkFuture = gcThreadPool.submit([this,frame]()->void {
            for (auto stackSlot = (*frame)->stack.cbegin(); stackSlot != (*frame)->stack.cend(); ++stackSlot) {
                this->mark(*stackSlot);
            }
        });
        
        localMarkFuture = gcThreadPool.submit([this, frame]()->void {
            for (auto localSlot = (*frame)->stack.cbegin(); localSlot != (*frame)->stack.cend(); ++localSlot) {
                this->mark(*localSlot);
            }
        });

    }
    if(!frames.empty()) {
        stackMarkFuture.get();
        localMarkFuture.get();
    }
    sweep();
    objectBitmap.clear();
    arrayBitmap.clear();
}