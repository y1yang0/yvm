#ifndef YVM_JAVAHEAP_H
#define YVM_JAVAHEAP_H

#include "JavaType.h"
#include <map>
#include <vector>
#include <mutex>
#include "ObjectMonitor.h"
#include "Utils.h"


using namespace std;

/**
 * \brief Java heap holds instance fields data which object referred to, and array component data 
 */
class JavaHeap {
    friend class CodeExecution;
    friend class GC;

public:
    JavaHeap() = default;
    ~JavaHeap();

    JObject* createObject(const JavaClass& javaClass);
    JArray* createPODArray(int atype, int length);
    JArray* createObjectArray(const JavaClass& jc, int length);
    JArray* createCharArray(const char* source, int length);


    /**
     * \brief Get the specified non-static field by given offset. *You should call this method
     * only if you are clearly about the memory layout of an object.*
     * \param object The object reference which contains the field you specified by given field offset
     * \param fieldOffset The object's field offset
     * \return The specified field if it was existed
     */
    JType* getObjectFieldByOffset(const JObject& object, size_t fieldOffset);
    void putObjectFieldByOffset(const JObject& object, size_t fieldOffset, JType* value);

    inline  JType* getObjectFieldByName(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
        JObject* object) {
        return getObjectFieldByNameImpl(parsedJc, fieldName, fieldDescriptor, object, 0);
    }
    inline void putObjectFieldByName(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
        JObject* object, JType* value) {
        putObjectFieldByNameImpl(parsedJc, fieldName, fieldDescriptor, object, value, 0);
    }

    void putArrayItem(const JArray& array, size_t index, JType* value);
    JType* getArrayItem(const JArray& array, size_t index);

    bool hasObjectMonitor(const JType* ref);

    void createObjectMonitor(const JType* ref);

    ObjectMonitor* findObjectMonitor(const JType* ref);

    auto& getObjectFieldsByRef(JObject* object) { lock_guard<recursive_mutex> lockMA(heapMutex); return (objheap.find(object->offset))->second; }
    auto& getObjectFieldsByOffset(size_t offset) { lock_guard<recursive_mutex> lockMA(heapMutex); return (objheap.find(offset))->second; }
    auto& getArrayItemsByRef(JArray* array) { lock_guard<recursive_mutex> lockMA(heapMutex); return (arrheap.find(array->offset))->second; }
    auto& getArrayItemsByOffset(size_t offset) { lock_guard<recursive_mutex> lockMA(heapMutex); return (arrheap.find(offset))->second; }
    void removeObjectByOffset(size_t offset) { lock_guard<recursive_mutex> lockMA(heapMutex); objheap.erase(objheap.find(offset)); }
    void removeArrayByOffset(size_t offset) { lock_guard<recursive_mutex> lockMA(heapMutex); arrheap.erase(arrheap.find(offset)); }
    void removeArrayByRef(const JArray* arr);
    void removeObjectByRef(const JObject* obj);

private:
    void createSuperFields(const JavaClass& javaClass, const JObject* object);
    /**
    * \brief Get the specified non-static field by given field name and descriptor. This is the usual
    * way to get field data of an object.
    * \param parsedJc The java class which contains the specified field
    * \param fieldName Field name
    * \param fieldDescriptor Field descriptor
    * \param object object The object reference which contains the field you specified by given field
    * name and field descriptor
    * \param offset YOU SHOULD NEVER ASSIGNE THIS ARGUMENT EXPECT 0. IT'S USED TO IMPLEMENT THIS METHOD ONLY.
    * \return The specified field if it was existed
    */
    JType* getObjectFieldByNameImpl(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
        JObject* object, size_t offset = 0);
    void putObjectFieldByNameImpl(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
        JObject* object, JType* value, size_t offset = 0);

private:
    std::recursive_mutex heapMutex;
    map<size_t, vector<JType*>> objheap;
    map<size_t, pair<size_t, JType**>> arrheap;
    map<size_t, ObjectMonitor*> monitorheap;
};


#endif // YVM_JAVAHEAP_H
