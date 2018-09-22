#include "JavaClass.h"
#include "RuntimeEnv.h"
#include "Utils.h"

std::string javastring2stdtring(JObject* objectref) {
    if (objectref == nullptr) {
        return std::string();
    }

    JArray* chararr = dynamic_cast<JArray*>(
        yrt.jheap->getFieldByName(objectref->jc, "value", "[C", objectref));

    std::string str;
    for (int i = 0; i < chararr->length; i++) {
        JInt* ch = dynamic_cast<JInt*>(yrt.jheap->getElement(*chararr, i));
        str += (char)ch->val;
    }
    return str;
}

JType* cloneValue(JType* value) {
    if (value == nullptr) {
        return nullptr;
    }
    JType* dupvalue{};
    if (typeid(*value) == typeid(JDouble)) {
        dupvalue = new JDouble();
        dynamic_cast<JDouble*>(dupvalue)->val =
            dynamic_cast<JDouble*>(value)->val;
    } else if (typeid(*value) == typeid(JFloat)) {
        dupvalue = new JFloat();
        dynamic_cast<JFloat*>(dupvalue)->val =
            dynamic_cast<JFloat*>(value)->val;
    } else if (typeid(*value) == typeid(JInt)) {
        dupvalue = new JInt();
        dynamic_cast<JInt*>(dupvalue)->val = dynamic_cast<JInt*>(value)->val;
    } else if (typeid(*value) == typeid(JLong)) {
        dupvalue = new JDouble();
        dynamic_cast<JDouble*>(dupvalue)->val =
            dynamic_cast<JDouble*>(value)->val;
    } else if (typeid(*value) == typeid(JObject)) {
        dupvalue = new JObject();
        dynamic_cast<JObject*>(dupvalue)->jc =
            dynamic_cast<JObject*>(value)->jc;
        dynamic_cast<JObject*>(dupvalue)->offset =
            dynamic_cast<JObject*>(value)->offset;
    } else if (typeid(*value) == typeid(JArray)) {
        dupvalue = new JArray();
        dynamic_cast<JArray*>(dupvalue)->length =
            dynamic_cast<JArray*>(value)->length;
        dynamic_cast<JArray*>(dupvalue)->offset =
            dynamic_cast<JArray*>(value)->offset;
    } else {
        SHOULD_NOT_REACH_HERE
    }
    return dupvalue;
}

bool hasInheritanceRelationship(const JavaClass* source,
                                const JavaClass* super) {
    if (source->getClassName() == super->getClassName()) {
        return true;
    }
    if (source->hasSuperClass()) {
        return hasInheritanceRelationship(
            yrt.ma->loadClassIfAbsent(source->getSuperClassName()), super);
    }
    return false;
}

void registerNativeMethod(const char* className, const char* name,
                          const char* descriptor,
                          JType* (*func)(RuntimeEnv* env)) {
    std::string methodName(className);
    methodName.append(".");
    methodName.append(name);
    methodName.append(".");
    methodName.append(descriptor);
    yrt.nativeMethods.insert(std::make_pair(methodName, func));
}


JType* determineBasicType(const std::string& type) {
    if (IS_FIELD_INT(type) || IS_FIELD_BYTE(type) || IS_FIELD_CHAR(type) ||
        IS_FIELD_SHORT(type) || IS_FIELD_BOOL(type)) {
        return new JInt;
    }
    if (IS_FIELD_DOUBLE(type)) {
        return new JDouble;
    }
    if (IS_FIELD_FLOAT(type)) {
        return new JFloat;
    }
    if (IS_FIELD_LONG(type)) {
        return new JLong;
    }
    return nullptr;
}

std::string peelClassNameFrom(const std::string& descriptor) {
    if (descriptor[0] != 'L') {
        return std::string("");
    }
    std::string className(descriptor);
    return className.substr(1, className.length() - 1 - 1);
}

std::string peelArrayComponentTypeFrom(const std::string& descriptor) {
    int i = 0;
    while (descriptor[i] == '[') {
        i++;
    }
    std::string arrayComponentTypeName(descriptor);
    return arrayComponentTypeName.substr(i,
                                         arrayComponentTypeName.length() - i);
}

std::tuple<int, std::vector<int>> peelMethodParameterAndType(
    const std::string& descriptor) {
    std::vector<int> parameters;

    int i = 0;
    while (descriptor[i] != ')') {
        switch (descriptor[i]) {
            case 'B':
                parameters.push_back(T_BYTE);
                break;
            case 'C':
                parameters.push_back(T_CHAR);
                break;
            case 'D':
                parameters.push_back(T_DOUBLE);
                break;
            case 'F':
                parameters.push_back(T_FLOAT);
                break;
            case 'I':
                parameters.push_back(T_INT);
                break;
            case 'J':
                parameters.push_back(T_LONG);
                break;
            case 'S':
                parameters.push_back(T_SHORT);
                break;
            case 'Z':
                parameters.push_back(T_BOOLEAN);
                break;
            case '[': {
                int arrayComponentType = ++i;
                while (descriptor[arrayComponentType] == '[') {
                    arrayComponentType++;
                }
                i = arrayComponentType;
                parameters.push_back(T_EXTRA_ARRAY);
                break;
            }
            case 'L': {
                int objectType = i++;
                while (descriptor[objectType] != ';') {
                    objectType++;
                }
                i = objectType;
                parameters.push_back(T_EXTRA_OBJECT);
                break;
            }
        }
        i++;
    }
    while (descriptor[i]) {
        switch (descriptor[i]) {
            case 'B':
                return std::make_tuple(T_BYTE, parameters);
            case 'C':
                return std::make_tuple(T_CHAR, parameters);
            case 'D':
                return std::make_tuple(T_DOUBLE, parameters);
            case 'F':
                return std::make_tuple(T_FLOAT, parameters);
            case 'I':
                return std::make_tuple(T_INT, parameters);
            case 'J':
                return std::make_tuple(T_LONG, parameters);
            case 'S':
                return std::make_tuple(T_SHORT, parameters);
            case 'Z':
                return std::make_tuple(T_BOOLEAN, parameters);
            case 'V':
                return std::make_tuple(T_EXTRA_VOID, parameters);
            case '[': {
                int arrayComponentType = i++;
                while (descriptor[arrayComponentType] != '[') {
                    arrayComponentType++;
                }
                i = arrayComponentType;

                return std::make_tuple(T_EXTRA_OBJECT, parameters);
            }
            case 'L': {
                int objectType = i++;
                while (descriptor[objectType] != ';') {
                    objectType++;
                }
                i = objectType;

                return std::make_tuple(T_EXTRA_OBJECT, parameters);
            }
        }
        i++;
    }
    SHOULD_NOT_REACH_HERE
}
