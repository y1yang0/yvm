#include "Descriptor.h"
#include "Opcode.h"
#include "Utils.h"

JType * determineBasicType(const char * type) {
    if (IS_FIELD_INT(type) || IS_FIELD_BYTE(type) || IS_FIELD_CHAR(type) || IS_FIELD_SHORT(type) || IS_FIELD_BOOL(type)) {
        return new JInt;
    }
    else if (IS_FIELD_DOUBLE(type)) {
        return new JDouble;
    }
    else if (IS_FIELD_FLOAT(type)) {
        return new JFloat;
    }
    else if (IS_FIELD_LONG(type)) {
        return new JLong;
    }
    return nullptr;
}

std::string peelClassNameFrom(const char * descriptor) {
    if (descriptor[0] != 'L') {
        return std::string("");
    }
    std::string className(descriptor);
    return className.substr(1, className.length() - 1 - 1);
}

std::string peelArrayComponentTypeFrom(const char * descriptor) {
    int i = 0;
    while(descriptor[i]=='[') {
        i++;
    }
    std::string arrayComponentTypeName(descriptor);
    return arrayComponentTypeName.substr(i, arrayComponentTypeName.length() - i);
}

std::tuple<int, std::vector<int>>  peelMethodParameterAndType(const char * descriptor){
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
            case '[':{
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
