#ifndef YVM_PARSEUTIL_H
#define YVM_PARSEUTIL_H

#include <cassert>
#include <string>
#include <vector>
#include "../interpreter/Internal.h"
#include "../runtime/JavaType.h"
#include "../runtime/RuntimeEnv.h"

//--------------------------------------------------------------------------------
// Force the compiler to inline decorated functions
//--------------------------------------------------------------------------------
#ifdef _MSC_VER_
#define forceinline __forceinline
#elif (defined __GNUC__) || (defined __clang__)
#define forceinline __inline__ __attribute__((always_inline))
#else
#define forceinline
#endif

#if (defined __GNUC__) || (defined __clang__)
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif
//--------------------------------------------------------------------------------
// Utilities that widely used in all components
//--------------------------------------------------------------------------------
#define FOR_EACH(iter, var) for (decltype(var) iter = 0; iter < var; iter++)
#define SHOULD_NOT_REACH_HERE                                     \
    assert(true,                                                  \
           "Program should not reach here, it's a vital logical " \
           "error\n");

//--------------------------------------------------------------------------------
// Convert java.lang.String object to a stl string
//--------------------------------------------------------------------------------
std::string javastring2stdtring(JObject* objectref);

//--------------------------------------------------------------------------------
// These functions were merely used by code execution engine.
//--------------------------------------------------------------------------------
JType* cloneValue(JType* value);
bool hasInheritanceRelationship(const JavaClass* source,
                                const JavaClass* super);
void registerNativeMethod(const char* className, const char* name,
                          const char* descriptor,
                          JType* (*func)(RuntimeEnv*, JType**, int));

inline u1 consumeU1(const u1* code, u4& opidx) {
    const u1 byte = code[++opidx];
    return byte;
}

inline u2 consumeU2(const u1* code, u4& opidx) {
    const u1 indexbyte1 = code[++opidx];
    const u1 indexbyte2 = code[++opidx];
    const u2 index = (indexbyte1 << 8) | indexbyte2;
    return index;
}

inline u4 consumeU4(const u1* code, u4& opidx) {
    const u1 byte1 = code[++opidx];
    const u1 byte2 = code[++opidx];
    const u1 byte3 = code[++opidx];
    const u1 byte4 = code[++opidx];
    const u4 res = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | (byte4);
    return res;
}

//--------------------------------------------------------------------------------
// These functions could peel method/field descriptors which described as jvm
// specification to a desirable representation or check whether it's a specific
// type.
//--------------------------------------------------------------------------------
#define IS_SIGNATURE_POLYMORPHIC_METHOD(className, methodName) \
    (className == "java/lang/invoke/MethodHandle" &&           \
     (methodName == "invokeExtract" || methodName == "invoke"))

#define IS_TYPE(type, strRepresentation)                          \
    inline bool IS_FIELD_##type(const std::string& descriptor) {  \
        return descriptor == strRepresentation;                   \
    }                                                             \
    inline bool IS_METHOD_##type(const std::string& descriptor) { \
        return descriptor == strRepresentation == 0;              \
    }

#define IS_REF_TYPE(type, c)                                          \
    inline bool IS_FIELD_REF_##type(const std::string& descriptor) {  \
        return descriptor[0] == c;                                    \
    }                                                                 \
    inline bool IS_METHOD_REF_##type(const std::string& descriptor) { \
        return descriptor[0] == c;                                    \
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

JType* determineBasicType(const std::string& type);

std::string peelClassNameFrom(const std::string& descriptor);

std::string peelArrayComponentTypeFrom(const std::string& descriptor);

std::tuple<int, std::vector<int> > peelMethodParameterAndType(
    const std::string& descriptor);

#endif  // YVM_PARSEUTIL_H
