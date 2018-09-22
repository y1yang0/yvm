#include "AccessFlag.h"
#include "JavaClass.h"
#include "JavaHeap.hpp"
#include "JavaType.h"
#include "NativeMethod.h"
#include "Utils.h"

#pragma warning(disable : 4715)

using namespace std;

// object creation and array creation
void JavaHeap::createSuperFields(const JavaClass& javaClass,
                                 const JObject* object) {
    lock_guard<recursive_mutex> lock(objMtx);

    if (javaClass.raw.superClass != 0) {
        const JavaClass* superClass =
            yrt.ma->findJavaClass(javaClass.getSuperClassName());
        FOR_EACH(i, superClass->raw.fieldsCount) {
            // Note that we have already created static field variables when the
            // javaClass is linked into jvm (YVM::linkClass()) so we ignore all
            // static fields here.
            const string& descriptor = superClass->getString(
                superClass->raw.fields[i].descriptorIndex);

            if (IS_FIELD_REF_CLASS(descriptor)) {
                // Special handling for field whose type is another class
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JObject* fieldObject = nullptr;
                    objectContainer.find(object->offset).push_back(fieldObject);
                }
            } else if (IS_FIELD_REF_ARRAY(descriptor)) {
                // Special handling for field whose type is array. We create a
                // null JArray as a placeholder since we don't know more
                // information about size of array, so we defer to allocate
                // memory while meeting opcodes [newarray]/[multinewarray]
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JArray* uninitializedArray = nullptr;
                    objectContainer.find(object->offset)
                        .push_back(uninitializedArray);
                }
            } else {
                // Otherwise it's a basic type. We insert it into instance's
                // field or its class static field by determining its access
                // flag
                if (!IS_FIELD_STATIC(superClass->raw.fields[i].accessFlags)) {
                    JType* basicField = determineBasicType(descriptor);
                    objectContainer.find(object->offset).push_back(basicField);
                }
            }
        }
        if (superClass->raw.superClass != 0) {
            createSuperFields(*superClass, object);
        }
    }
}

// create an object on the heap. This is the only way to create objects in
// the yvm
JObject* JavaHeap::createObject(const JavaClass& javaClass) {
    lock_guard<recursive_mutex> lock(objMtx);

    JObject* object = new JObject;
    object->jc = &javaClass;
    object->offset = objectContainer.place();
    vector<JType*> instanceFields;

    FOR_EACH(fieldOffset, javaClass.raw.fieldsCount) {
        // Note that we have already created static field variables when the
        // javaClass is linked into jvm (YVM::linkClass()) so we ignore all
        // static fields here.
        const string& descriptor = javaClass.getString(
            javaClass.raw.fields[fieldOffset].descriptorIndex);

        if (IS_FIELD_REF_CLASS(descriptor)) {
            // Special handling for field whose type is another class
            if (!IS_FIELD_STATIC(
                    javaClass.raw.fields[fieldOffset].accessFlags)) {
                JObject* fieldObject = nullptr;
                instanceFields.push_back(fieldObject);
            }
        } else if (IS_FIELD_REF_ARRAY(descriptor)) {
            // Special handling for field whose type is array. We create a null
            // JArray as a placeholder since we don't know more information
            // about size of array, so we defer to allocate memory while meeting
            // opcodes [newarray]/[multinewarray]
            if (!IS_FIELD_STATIC(
                    javaClass.raw.fields[fieldOffset].accessFlags)) {
                JArray* uninitializedArray = nullptr;
                instanceFields.push_back(uninitializedArray);
            }
        } else {
            // Otherwise it's a basic type. We insert it into instance's field
            // or its class static field by determining its access flag
            if (!IS_FIELD_STATIC(
                    javaClass.raw.fields[fieldOffset].accessFlags)) {
                JType* basicField = determineBasicType(descriptor);
                instanceFields.push_back(basicField);
            }
        }
    }
    objectContainer.find(object->offset) = instanceFields;
    createSuperFields(javaClass, object);
    return object;
}

JArray* JavaHeap::createPODArray(int atype, int length) {
    lock_guard<recursive_mutex> lock(arrMtx);

    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrayContainer.place();

    JType** items = new JType*[arr->length];
    switch (atype) {
        case T_FLOAT:
            FOR_EACH(i, length) { items[i] = new JFloat; }
            arrayContainer.find(arr->offset) = make_pair(length, items);
            return arr;
        case T_DOUBLE:
            FOR_EACH(i, length) { items[i] = new JDouble; }
            arrayContainer.find(arr->offset) = make_pair(length, items);
            return arr;
        case T_BOOLEAN:
        case T_CHAR:
        case T_BYTE:
        case T_SHORT:
        case T_INT:
            FOR_EACH(i, length) { items[i] = new JInt; }
            arrayContainer.find(arr->offset) = make_pair(length, items);
            return arr;
        case T_LONG:
            FOR_EACH(i, length) { items[i] = new JLong; }
            arrayContainer.find(arr->offset) = make_pair(length, items);
            return arr;
        default:
            return nullptr;
    }
}

JArray* JavaHeap::createObjectArray(const JavaClass& jc, int length) {
    lock_guard<recursive_mutex> lock(arrMtx);
    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrayContainer.place();

    JType** items = new JType*[arr->length];
    FOR_EACH(i, length) { items[i] = createObject(jc); }
    arrayContainer.find(arr->offset) = make_pair(length, items);
    return arr;
}

JArray* JavaHeap::createCharArray(const string& source, size_t length) {
    lock_guard<recursive_mutex> lock(arrMtx);
    JArray* arr = new JArray;
    arr->length = length;
    arr->offset = arrayContainer.place();

    JType** items = new JType*[arr->length];
    FOR_EACH(i, length) { items[i] = new JInt(source[i]); }
    arrayContainer.find(arr->offset) = make_pair(length, items);
    return arr;
}

// get object's field by name and descriptor or set object's field by given
// value note that we should not use object->jc to instead the first
// argument since we might lookup a field in base class, while the derive
// class has the same name
// For example, there might be two classes, which one has field_a, and another
// has the same field  name and the later(ClassA) extends previous
// class(ClassB), now if we want to get/set ClassB.field we must set
// desireLookup as ClassA, and currentLookup for object->jc, which means starts
// lookup from current object related class(ClassB)
JType* JavaHeap::getFieldByNameImpl(const JavaClass* desireLookup,
                                    const JavaClass* currentLookup,
                                    const string& name,
                                    const string& descriptor, JObject* object,
                                    size_t offset /*= 0*/) {
    lock_guard<recursive_mutex> lock(objMtx);
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, currentLookup->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(currentLookup->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const string& n = currentLookup->getString(
                currentLookup->raw.fields[i].nameIndex);
            const string& d = currentLookup->getString(
                currentLookup->raw.fields[i].descriptorIndex);
            if (n == name && d == descriptor && desireLookup == currentLookup) {
                return objectContainer.find(object->offset)[i + offset];
            }
        }
    }
    if (currentLookup->raw.superClass != 0) {
        return getFieldByNameImpl(
            desireLookup,
            yrt.ma->findJavaClass(currentLookup->getSuperClassName()), name,
            descriptor, object, offset + howManyNonStaticFields);
    }
    return nullptr;
}

void JavaHeap::putFieldByNameImpl(const JavaClass* desireLookup,
                                  const JavaClass* currentLookup,
                                  const string& name, const string& descriptor,
                                  JObject* object, JType* value,
                                  size_t offset /*= 0*/) {
    lock_guard<recursive_mutex> lock(objMtx);
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, currentLookup->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(currentLookup->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const string& n = currentLookup->getString(
                currentLookup->raw.fields[i].nameIndex);
            const string& d = currentLookup->getString(
                currentLookup->raw.fields[i].descriptorIndex);
            if (n == name && d == descriptor && desireLookup == currentLookup) {
                objectContainer.find(object->offset)[i + offset] = value;
                return;
            }
        }
    }
    if (currentLookup->raw.superClass != 0) {
        putFieldByNameImpl(
            desireLookup,
            yrt.ma->findJavaClass(currentLookup->getSuperClassName()), name,
            descriptor, object, value, offset + howManyNonStaticFields);
    }
}