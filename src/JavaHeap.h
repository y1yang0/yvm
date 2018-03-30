#ifndef YVM_JAVAHEAP_H
#define YVM_JAVAHEAP_H

#include "JavaType.h"
#include <map>
#include <vector>


using namespace std;

/**
 * \brief Java heap holds instance fields data which object referred to, and array component data 
 */
class JavaHeap {
    friend class CodeExecution;
public:
    JavaHeap() = default;
    ~JavaHeap();

    JObject* createObject(const JavaClass& javaClass);
    JArray* createPODArray(int atype, int length);
    JArray* createObjectArray(const JavaClass& jc, int length);
    JArray* createCharArray(const char* source, int length);

    void putObjectField(const JObject& object, size_t fieldOffset, JType* value);
    void putArrayItem(const JArray& array, size_t index, JType* value);
    JType* getArrayItem(const JArray& array, size_t index);
    JType* getObjectField(const JObject& object, size_t fieldOffset);

    auto& getObject(JObject* object) {
        return (objheap.find(object->offset))->second;
    }

    auto& getArray(JArray* array) {
        return (arrheap.find(array->offset))->second;
    }

    void removeArray(const JArray * arr);

private:
    void createSuperFields(const JavaClass& javaClass, const JObject* object);

private:
    map<size_t, vector<JType*>> objheap;
    map<size_t, pair<size_t, JType**>> arrheap;
};


#endif // YVM_JAVAHEAP_H
