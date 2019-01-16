#ifndef _CALLSITE_H
#define _CALLSITE_H

#include "../runtime/JavaClass.h"

struct CallSite {
    explicit CallSite();

    bool isCallable() const { return callable; }

    static CallSite makeCallSite(const JavaClass* jc, MethodInfo* m);

    const JavaClass* jc;
    u2 accessFlags;
    u1* code;
    u4 codeLength;
    u2 maxStack;
    u2 maxLocal;
    u2 exceptionLen;
    ATTR_Code::ExceptionTable* exception;
    bool callable;
};

#endif  // !_CALLSITE_H
