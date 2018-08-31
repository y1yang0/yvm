#ifndef YVM_JAVAHEAP_H
#define YVM_JAVAHEAP_H

#include <map>
#include <mutex>
#include <vector>
#include "GC.h"
#include "JavaType.h"
#include "ObjectMonitor.h"
#include "Utils.h"

using namespace std;

using InternalObject = vector<JType*>;
using InternalArray = pair<size_t, JType**>;
using InternalMonitor = ObjectMonitor*;

template <typename Type>
class Container {
    friend class ConcurrentGC;

public:
    explicit Container() = default;
    virtual ~Container() {}
    virtual size_t place() {
        size_t lastOffset = 0;
        if (!container.empty()) {
            lastOffset = (--container.end())->first;
        }
        container.insert(make_pair(lastOffset + 1, Type{}));
        return lastOffset + 1;
    }
    void remove(size_t offset) {
        if (container.find(offset) != container.end()) {
            container.erase(offset);
        }
    }
    Type& find(size_t offset) { return container.find(offset)->second; }
    bool has(size_t offset) {
        return container.find(offset) != container.end();
    }

protected:
    auto& getContainer() { return container; }

private:
    map<size_t, Type, less<>, HeapAllocator<pair<const size_t, Type>>>
        container;
};

//--------------------------------------------------------------------------------
// The ArrayContainer manages array's elements as well as object pool
//
// [1]  ->   <3, [field_a, field_b, field_c]>
// [2]  ->   <0, []>
// [3]  ->   <2, [field_a,field_b]>
// [4]  ->   <1, [field_a]>
// [..] ->   <..,[...]>
//--------------------------------------------------------------------------------
struct ArrayContainer : public Container<InternalArray> {
    ~ArrayContainer() override {
        for (auto intPairPair : getContainer()) {
            for (size_t i = 0; i < intPairPair.second.first; i++) {
                delete intPairPair.second.second[i];
            }
            delete[] intPairPair.second.second;
        }
    }
};
//--------------------------------------------------------------------------------
// The ObjectContainer manages object's fields data, the key also the
// only way to identify an object is the offset, you can use offset to get
// object's fields data, push a field data into objects which specified by
// offset and place a new area to store object fields Here is the internal
// construction:
//
// [1]  ->  [field_a, field_b, field_c]
// [2]  ->  []
// [3]  ->  [field_a,field_b]
// [4]  ->  [field_a]
// [..] ->  [...]
//--------------------------------------------------------------------------------
struct ObjectContainer : public Container<InternalObject> {
    ~ObjectContainer() override {
        for (auto intVecPair : getContainer()) {
            for (auto ptr : intVecPair.second) {
                delete ptr;
            }
        }
    }
};

//--------------------------------------------------------------------------------
// MonitorContainer manages synchronous block monitors as well as the
// above
//
// [1]  ->   ObjectMonitor*
// [2]  ->   ObjectMonitor*
// [3]  ->   ObjectMonitor*
// [4]  ->   ObjectMonitor*
// [..] ->   ObjectMonitor*// MonitorContainer manages synchronous block
// monitors as well as the above
//
// [1]  ->   ObjectMonitor*
// [2]  ->   ObjectMonitor*
// [3]  ->   ObjectMonitor*
// [4]  ->   ObjectMonitor*
// [..] ->   ObjectMonitor*
//--------------------------------------------------------------------------------
struct MonitorContainer : public Container<InternalMonitor> {
    ~MonitorContainer() override {
        for (auto intMonitorPair : getContainer()) {
            delete intMonitorPair.second;
        }
    }
    size_t place() override {
        size_t lastOffset = 0;
        if (!getContainer().empty()) {
            lastOffset = (--getContainer().end())->first;
        }
        getContainer().insert(make_pair(lastOffset + 1, new ObjectMonitor()));
        return lastOffset;
    }
};
//--------------------------------------------------------------------------------
// Java heap holds instance's fields data which object referred to and elements
// of an array. This is the core component of yvm, almost every memory
// storage/access/deletion took place here.
//--------------------------------------------------------------------------------
class JavaHeap {
    friend class CodeExecution;
    friend class ConcurrentGC;

public:
    JavaHeap() = default;

    JObject* createObject(const JavaClass& javaClass);
    JArray* createPODArray(int atype, int length);
    JArray* createObjectArray(const JavaClass& jc, int length);
    JArray* createCharArray(const string& source, size_t length);

    auto getFieldByName(const JavaClass* jc, const string& name,
                        const string& descriptor, JObject* object) {
        return getFieldByNameImpl(jc, object->jc, name, descriptor, object, 0);
    }
    void putFieldByName(const JavaClass* jc, const string& name,
                        const string& descriptor, JObject* object,
                        JType* value) {
        putFieldByNameImpl(jc, object->jc, name, descriptor, object, value, 0);
    }
    void putFieldByOffset(const JObject& object, size_t fieldOffset,
                                    JType* value) {
        lock_guard<recursive_mutex> lock(objMtx);
        objectContainer.find(object.offset)[fieldOffset] = value;
    }
    auto getFieldByOffset(const JObject& object, size_t fieldOffset) {
        lock_guard<recursive_mutex> lock(objMtx);
        return objectContainer.find(object.offset)[fieldOffset];
    }
    auto getFields(JObject* object) {
        lock_guard<recursive_mutex> lockMA(objMtx);
        return objectContainer.find(object->offset);
    }

    void putElement(const JArray& array, size_t index, JType* value) {
        lock_guard<recursive_mutex> lock(arrMtx);
        arrayContainer.find(array.offset).second[index] = value;
    }
    auto getElement(const JArray& array, size_t index) {
        lock_guard<recursive_mutex> lock(arrMtx);
        return arrayContainer.find(array.offset).second[index];
    }
    auto getElements(JArray* array) {
        lock_guard<recursive_mutex> lockMA(arrMtx);
        return arrayContainer.find(array->offset);
    }

    void removeArray(size_t offset) {
        lock_guard<recursive_mutex> lock(arrMtx);
        arrayContainer.remove(offset);
    }
    void removeObject(size_t offset) {
        lock_guard<recursive_mutex> lock(objMtx);
        objectContainer.remove(offset);
    }

    bool hasMonitor(const JType* ref) {
        lock_guard<recursive_mutex> lock(monitorMtx);
        return monitorContainer.has(dynamic_cast<const JObject*>(ref)->offset);
    }
    auto createMonitor() {
        lock_guard<recursive_mutex> lock(monitorMtx);
        return monitorContainer.place();
    }
    auto findMonitor(const JType* ref) {
        lock_guard<recursive_mutex> lock(monitorMtx);
        return monitorContainer.find(dynamic_cast<const JObject*>(ref)->offset);
    }

private:
    void createSuperFields(const JavaClass& javaClass, const JObject* object);

    JType* getFieldByNameImpl(const JavaClass* desireLookup,
                              const JavaClass* currentLookup,
                              const string& name, const string& descriptor,
                              JObject* object, size_t offset = 0);
    void putFieldByNameImpl(const JavaClass* desireLookup,
                            const JavaClass* currentLookup, const string& name,
                            const string& descriptor, JObject* object,
                            JType* value, size_t offset = 0);

private:
    ObjectContainer objectContainer;
    ArrayContainer arrayContainer;
    MonitorContainer monitorContainer;

    recursive_mutex objMtx;
    recursive_mutex arrMtx;
    recursive_mutex monitorMtx;
};
#endif  // YVM_JAVAHEAP_H
