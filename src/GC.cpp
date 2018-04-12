#include "GC.h"

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

void GC::sweep() const {
    lock_guard<recursive_mutex> lock(yrt.jheap->heapMutex);
    for(auto pos = yrt.jheap->objheap.begin();pos!=yrt.jheap->objheap.end();) {
        // If we can not find active object in object bitmap then clear it
        if(objectBitmap.find(pos->first)==objectBitmap.cend()) {
            yrt.jheap->objheap.erase(pos++);
        }else {
            ++pos;
        }
    }

    for (auto pos = yrt.jheap->arrheap.begin(); pos != yrt.jheap->arrheap.end();) {
        // If we can not find active object in array bitmap then clear it
        if (arrayBitmap.find(pos->first) == arrayBitmap.cend()) {
            yrt.jheap->arrheap.erase(pos++);
        }
        else {
            ++pos;
        }
    }
}

void GC::markAndSweep() {  
    for(auto frame = frames.cbegin();frame!=frames.cend();++frame) {
        for(auto stackSlot = (*frame)->stack.cbegin();stackSlot!=(*frame)->stack.cend();++stackSlot) {
            mark(*stackSlot);
        }

        for (auto localSlot = (*frame)->stack.cbegin(); localSlot != (*frame)->stack.cend(); ++localSlot){
            mark(*localSlot);
        }
    }
    sweep();
    objectBitmap.clear();
    arrayBitmap.clear();
}



