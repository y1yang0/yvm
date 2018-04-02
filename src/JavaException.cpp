#include "JavaException.h"
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

void  StackTrace::setThrowExceptionInfo(JObject * throwableObject) {
    throwExceptionClass = throwableObject->jc;
    JObject * messageField = dynamic_cast<JObject*>(
        yrt.jheap->getObjectFieldByName(
            const_cast<JavaClass*>(throwableObject->jc), "message", "Ljava/lang/String;", throwableObject, 0)
        );
    exceptionStackTrace.push_back(Converter::javastring2stdtring(messageField).c_str());
}
