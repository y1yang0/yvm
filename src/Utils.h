#ifndef YVM_PARSEUTIL_H
#define YVM_PARSEUTIL_H

#include <string>
#include "JavaType.h"
#include "RuntimeEnv.h"
#include "Internal.h"

/**
 * \brief Utilities that widely used in all aspects
 */

#define FOR_EACH(iter, var) for (decltype(var) iter = 0; iter < var; iter++)
#define SHOULD_NOT_REACH_HERE                                            \
    static_assert(true,                                                  \
                  "Program should not reach here, it's a vital logical " \
                  "error\n");

struct Converter {
    /**
     * \brief Convert java.lang.String object to a stl string
     */
    static std::string javastring2stdtring(JObject* objectref);
};

/**
 * \brief Utilities for code execution engine
 */

JType* cloneValue(JType* value);
bool hasInheritanceRelationship(const JavaClass* source,
                                const JavaClass* super);
void registerNativeMethod(const char* className, const char* name,
                          const char* descriptor,
                          JType* (*func)(RuntimeEnv* env));

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
#endif  // YVM_PARSEUTIL_H
