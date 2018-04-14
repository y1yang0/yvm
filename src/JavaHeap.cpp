#include "JavaHeap.h"
#include "AccessFlag.h"
#include "JavaType.h"
#include "Descriptor.h"
#include "JavaClass.h"
#include "Opcode.h"
#include "Utils.h"
#include "NativeMethod.h"

#pragma warning (disable: 4715) 

JavaHeap::~JavaHeap() {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);

    for (auto start = objheap.begin(); start != objheap.end(); ++start) {
        for (auto* item : start->second) {
            delete item;
        }
    }
    for (auto start = arrheap.begin(); start != arrheap.end(); ++start) {
        FOR_EACH(i, start->second.first) {
            delete start->second.second[i];
        }
        delete[] start->second.second;
    }
}

void JavaHeap::putObjectFieldByOffset(const JObject& object, size_t fieldOffset, JType* value) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    auto fields = objheap.find(object.offset);
    fields->second[fieldOffset] = new JType;
    fields->second[fieldOffset] = value;
}

void JavaHeap::putArrayItem(const JArray& array, size_t index, JType* value) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    arrheap.find(array.offset)->second.second[index] = value;
}

JType* JavaHeap::getArrayItem(const JArray& array, size_t index) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    return arrheap.find(array.offset)->second.second[index];
}

JType* JavaHeap::getObjectFieldByOffset(const JObject& object, size_t fieldOffset) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    auto& fields = objheap.find(object.offset)->second;
    return fields[fieldOffset];
}

void JavaHeap::removeArrayByRef(const JArray* arr) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    if (arr == nullptr) {
        return;
    }
    auto pair = arrheap.find(arr->offset);
    for(size_t i=0;i<pair->second.first;i++) {
        delete pair->second.second[i];
    }
    delete[] pair->second.second;
    arrheap.erase(pair);
}

void JavaHeap::removeObjectByRef(const JObject* obj) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    if (obj == nullptr) {
        return;
    }
    objheap.erase(objheap.find(obj->offset));
}

void JavaHeap::createSuperFields(const JavaClass& javaClass, const JObject* object) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);

    if (javaClass.raw.superClass != 0) {
        const JavaClass* superClass = yrt.ma->findJavaClass(javaClass.getSuperClassName());
        FOR_EACH(i, superClass->raw.fieldsCount) {
            // Note that we have already created static field variables when the javaClass is 
            // linked into jvm (YVM::linkClass()) so we ignore all static fields here.
            const char* descriptor = superClass->getString(superClass->raw.fields[i].descriptorIndex);

            if (IS_FIELD_REF_CLASS(descriptor)) {
                // Special handling for field whose type is another class
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JObject* fieldObject = nullptr;
                    yrt.jheap->objheap.find(object->offset)->second.push_back(fieldObject);
                }
            }
            else if (IS_FIELD_REF_ARRAY(descriptor)) {
                // Special handling for field whose type is array. We create a null JArray as a placeholder
                // since we dont know more information about size of array, so we defer to allocate memory
                // while meeting opcodes [newarray]/[multinewarray]
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JArray* uninitializedArray = nullptr;
                    yrt.jheap->objheap.find(object->offset)->second.push_back(uninitializedArray);
                }
            }
            else {
                // Otherwise it's a basic type. We insert it into instance's field or its class static field
                // by determining its access flag
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JType* basicField = determineBasicType(descriptor);
                    yrt.jheap->objheap.find(object->offset)->second.push_back(basicField);
                }
            }
        }
        if (superClass->raw.superClass != 0) {
            createSuperFields(*superClass, object);
        }
    }
}

JObject* JavaHeap::createObject(const JavaClass& javaClass) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);

    JObject* object = new JObject;
    object->jc = &javaClass;
    object->offset = objheap.empty() ? 0 : (--objheap.end())->first + 1;
    std::vector<JType*> instanceFields;

    FOR_EACH(fieldOffset, javaClass.raw.fieldsCount) {
        // Note that we have already created static field variables when the javaClass is 
        // linked into jvm (YVM::linkClass()) so we ignore all static fields here.
        const char* descriptor = javaClass.getString(javaClass.raw.fields[fieldOffset].descriptorIndex);

        if (IS_FIELD_REF_CLASS(descriptor)) {
            // Special handling for field whose type is another class
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JObject* fieldObject = nullptr;
                instanceFields.push_back(fieldObject);
            }
        }
        else if (IS_FIELD_REF_ARRAY(descriptor)) {
            // Special handling for field whose type is array. We create a null JArray as a placeholder
            // since we dont know more information about size of array, so we defer to allocate memory
            // while meeting opcodes [newarray]/[multinewarray]
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JArray* uninitializedArray = nullptr;
                instanceFields.push_back(uninitializedArray);
            }
        }
        else {
            // Otherwise it's a basic type. We insert it into instance's field or its class static field
            // by determining its access flag
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JType* basicField = determineBasicType(descriptor);
                instanceFields.push_back(basicField);
            }
        }
    }

    objheap.insert(std::make_pair(object->offset, instanceFields));
    createSuperFields(javaClass, object);
    return object;
}

JArray* JavaHeap::createPODArray(int atype, int length) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);

    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType** items = new JType*[arr->length];
    switch (atype) {
    case T_FLOAT:
        FOR_EACH(i, length) {
            items[i] = new JFloat;
        }
        arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
        return arr;
    case T_DOUBLE:
        FOR_EACH(i, length) {
            items[i] = new JDouble;
        }
        arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
        return arr;
    case T_BOOLEAN:
    case T_CHAR:
    case T_BYTE:
    case T_SHORT:
    case T_INT:
        FOR_EACH(i, length) {
            items[i] = new JInt;
        }
        arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
        return arr;
    case T_LONG:
        FOR_EACH(i, length) {
            items[i] = new JLong;
        }
        arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
        return arr;
    default:
        return nullptr;
    }
}

JArray* JavaHeap::createObjectArray(const JavaClass& jc, int length) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType** items = new JType*[arr->length];
    FOR_EACH(i, length) {
        items[i] = createObject(jc);
    }
    arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
    return arr;
}

JArray* JavaHeap::createCharArray(const char* source, int length) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType** items = new JType*[arr->length];
    FOR_EACH(i, length) {
        items[i] = new JInt(source[i]);
    }
    arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
    return arr;
}

JType* JavaHeap::getObjectFieldByNameImpl(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
                                      JObject* object, size_t offset) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const char* n = parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char* d = parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                return objheap.find(object->offset)->second.at(i + offset);
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        return getObjectFieldByNameImpl(yrt.ma->findJavaClass(parsedJc->getSuperClassName()), fieldName, fieldDescriptor,
                                    object, offset + howManyNonStaticFields);
    }
    return nullptr;
}

void JavaHeap::putObjectFieldByNameImpl(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
                                    JObject* object, JType* value, size_t offset) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const char* n = parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char* d = parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                objheap.find(object->offset)->second.at(offset + i) = value;
                return;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        putObjectFieldByNameImpl(yrt.ma->findJavaClass(parsedJc->getSuperClassName()), fieldName, fieldDescriptor, object,
                             value, offset + howManyNonStaticFields);
    }
}


bool JavaHeap::hasObjectMonitor(const JType* ref) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    if (typeid(*ref) == typeid(JObject)) {
        return monitorheap.find(dynamic_cast<const JObject*>(ref)->offset) != monitorheap.end();
    }
    if (typeid(*ref) == typeid(JArray)) {
        return monitorheap.find(dynamic_cast<const JArray*>(ref)->offset) != monitorheap.end();
    }
    SHOULD_NOT_REACH_HERE
}

void JavaHeap::createObjectMonitor(const JType* ref) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    if (typeid(*ref) == typeid(JObject)) {
        monitorheap.insert(std::make_pair(dynamic_cast<const JObject*>(ref)->offset, new ObjectMonitor()));
    }
    else if (typeid(*ref) == typeid(JArray)) {
        monitorheap.insert(std::make_pair(dynamic_cast<const JArray*>(ref)->offset, new ObjectMonitor()));
    }
    else {
        SHOULD_NOT_REACH_HERE
    }
}

ObjectMonitor* JavaHeap::findObjectMonitor(const JType* ref) {
    std::lock_guard<std::recursive_mutex> lockMA(heapMutex);
    if (typeid(*ref) == typeid(JObject)) {
        return monitorheap.find(dynamic_cast<const JObject*>(ref)->offset)->second;
    }
    if (typeid(*ref) == typeid(JArray)) {
        return monitorheap.find(dynamic_cast<const JArray*>(ref)->offset)->second;
    }
    SHOULD_NOT_REACH_HERE
}
