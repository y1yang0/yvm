#include "JavaHeap.h"
#include "AccessFlag.h"
#include "JavaType.h"
#include "Descriptor.h"
#include "JavaClass.h"
#include "Opcode.h"
#include "Utils.h"
#include "NativeMethod.h"

JavaHeap::~JavaHeap(){
    for (auto start = objheap.begin(); start != objheap.end(); start++) {
        for (auto * item : start->second) {
            delete item;
        }
    }
    for (auto start = arrheap.begin(); start != arrheap.end(); start++) {
        FOR_EACH(i, start->second.first) {
            delete start->second.second[i];
        }
        delete[] start->second.second;
    }
}

void JavaHeap::putObjectField(const JObject & object, size_t fieldOffset, JType * value) {
    auto fields = objheap.find(object.offset);
    fields->second[fieldOffset] = new JType;
    fields->second[fieldOffset] = value;
}

void JavaHeap::putArrayItem(const JArray & array, size_t index, JType * value){
    arrheap.find(array.offset)->second.second[index] = value;
}

JType * JavaHeap::getArrayItem(const JArray & array, size_t index){
    return arrheap.find(array.offset)->second.second[index];
}

JType * JavaHeap::getObjectField(const JObject & object, size_t fieldOffset){
    auto & fields = objheap.find(object.offset)->second;
    return fields[fieldOffset];
}

void JavaHeap::removeArray(const JArray* arr) {
    if(arr==nullptr) {
        return;
    }
    arrheap.erase(arrheap.find(arr->offset));
}

void JavaHeap::createSuperFields(const JavaClass & javaClass, const JObject * object) {
    if (javaClass.raw.superClass != 0) {
        const JavaClass * superClass = yrt.ma->findJavaClass((char*)javaClass.getSuperClassName());
        FOR_EACH(i, superClass->raw.fieldsCount) {
            // Note that we have already created static field variables when the javaClass is 
            // linked into jvm (YVM::linkClass()) so we ignore all static fields here.
            char * descriptor = (char*)superClass->getString(superClass->raw.fields[i].descriptorIndex);

            if (IS_FIELD_REF_CLASS(descriptor)) {
                // Special handling for field whose type is another class
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JObject * fieldObject = nullptr;
                    yrt.jheap->objheap.find(object->offset)->second.push_back(fieldObject);
                }
            }
            else if (IS_FIELD_REF_ARRAY(descriptor)) {
                // Special handling for field whose type is array. We create a null JArray as a placeholder
                // since we dont know more information about size of array, so we defer to allocate memory
                // while meeting opcodes [newarray]/[multinewarray]
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JArray * uninitializedArray = nullptr;
                    yrt.jheap->objheap.find(object->offset)->second.push_back(uninitializedArray);
                }
            }
            else {
                // Otherwise it's a basic type. We insert it into instance's field or its class static field
                // by determining its access flag
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JType * basicField = determineBasicType(descriptor);
                    yrt.jheap->objheap.find(object->offset)->second.push_back(basicField);
                }
            }
        }
        if (superClass->raw.superClass != 0) {
            createSuperFields(*superClass, object);
        }
    }
}

JObject * JavaHeap::createObject(const JavaClass & javaClass) {
    JObject * object = new JObject;
    object->jc = &javaClass;
    object->offset = objheap.empty() ? 0 : (--objheap.end())->first + 1;
    std::vector<JType*> instanceFields;

    FOR_EACH(fieldOffset, javaClass.raw.fieldsCount) {
        // Note that we have already created static field variables when the javaClass is 
        // linked into jvm (YVM::linkClass()) so we ignore all static fields here.
        char * descriptor = (char*)javaClass.getString(javaClass.raw.fields[fieldOffset].descriptorIndex);
       
        if (IS_FIELD_REF_CLASS(descriptor)) {
            // Special handling for field whose type is another class
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JObject * fieldObject = nullptr;
                instanceFields.push_back(fieldObject);
            }
        }
        else if (IS_FIELD_REF_ARRAY(descriptor)) {
            // Special handling for field whose type is array. We create a null JArray as a placeholder
            // since we dont know more information about size of array, so we defer to allocate memory
            // while meeting opcodes [newarray]/[multinewarray]
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JArray * uninitializedArray = nullptr;
                instanceFields.push_back(uninitializedArray);
            }
        }
        else {
            // Otherwise it's a basic type. We insert it into instance's field or its class static field
            // by determining its access flag
            if (!IS_FIELD_STATIC(javaClass.raw.fields[fieldOffset].accessFlags)) {
                JType * basicField = determineBasicType(descriptor);
                instanceFields.push_back(basicField);
            }
        }
    }

    objheap.insert(std::make_pair(object->offset, instanceFields));
    createSuperFields(javaClass, object);
    return object;
}

JArray * JavaHeap::createPODArray(int atype, int length){
    JArray * arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType ** items = new JType*[arr->length];
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

JArray * JavaHeap::createObjectArray(const JavaClass & jc, int length){
    JArray * arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType ** items = new JType*[arr->length];
    FOR_EACH(i, length) {
        items[i] = createObject(jc);
    }
    arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
    return arr;
}

JArray * JavaHeap::createCharArray(const char * source, int length) {
    JArray * arr = new JArray;
    arr->length = length;
    arr->offset = arrheap.empty() ? 0 : (--arrheap.end())->first + 1;

    JType ** items = new JType*[arr->length];
    FOR_EACH(i, length) {
        items[i] = new JInt(source[i]);
    }
    arrheap.insert(std::make_pair(arr->offset, std::make_pair(length, items)));
    return arr;
}