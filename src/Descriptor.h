#ifndef YVM_DESCRIPTOR_H
#define YVM_DESCRIPTOR_H

#include <string.h>
#include <string>
#include "JavaType.h"
#include <vector>
#include <tuple>

#define IS_TYPE(type,strRepresentation)   \
inline bool IS_FIELD_##type(const char * descriptor) {  \
    return strcmp(descriptor,strRepresentation) == 0;  \
}   \
inline bool IS_METHOD_##type(const char * descriptor) {  \
    return strcmp(descriptor,strRepresentation) == 0;  \
}

#define IS_REF_TYPE(type,c)   \
inline bool IS_FIELD_REF_##type(const char * descriptor) {  \
    return descriptor[0]==c;  \
}   \
inline bool IS_METHOD_REF_##type(const char * descriptor) {  \
    return descriptor[0]==c;  \
}

IS_TYPE(BYTE, "B")
IS_TYPE(CHAR, "C")
IS_TYPE(DOUBLE, "D")
IS_TYPE(FLOAT, "F")
IS_TYPE(INT, "I")
IS_TYPE(LONG, "J")
IS_TYPE(SHORT, "S")
IS_TYPE(BOOL, "Z")
IS_TYPE(VOID, "V")

IS_REF_TYPE(CLASS, 'L');
IS_REF_TYPE(ARRAY, '[');


JType* determineBasicType(const char* type);

std::string peelClassNameFrom(const char* descriptor);

std::string peelArrayComponentTypeFrom(const char* descriptor);

std::tuple<int, std::vector<int>> peelMethodParameterAndType(const char* descriptor);

#define IS_SIGNATURE_POLYMORPHIC_METHOD(className,methodName)\
    (strcmp(className,"java/lang/invoke/MethodHandle")==0 && \
    (strcmp(methodName,"invokeExtract")==0||strcmp(methodName,"invoke")==0))

#endif // !YVM_DESCRIPTOR_H
