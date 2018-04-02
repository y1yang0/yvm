#include "Exception.h"
#include "JavaClass.h"
#include <cstdio>
#include <cassert>

void StackTrace::printStackTrace() {
    assert(!exceptionStackTrace.empty());
    assert(throwExceptionClass != nullptr);

    printf("Thrown %s at %s()\n", throwExceptionClass->getClassName(), exceptionStackTrace[0]);
    !detailedMsg.empty() ? printf("Reason:%s\n", detailedMsg.c_str()) : printf("");

    int deep = 0;
    for (auto * x : exceptionStackTrace) {
        deep++;
        for(int i=0;i<deep;i++) {
            printf("-");
        }
        printf("By its caller %s()\n", x);
    }
}
