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
    for (size_t p = 1; p < exceptionStackTrace.size(); p++) {
        deep++;
        for (int i = 0; i < deep; i++) {
            printf("-");
        }
        printf("By its caller %s()\n", exceptionStackTrace[p]);
    }
}

void StackTrace::setThrowExceptionInfo(JObject* throwableObject) {
    throwExceptionClass = throwableObject->jc;
    auto* messageField = dynamic_cast<JObject*>(
        yrt.jheap->getObjectFieldByName(
            const_cast<JavaClass*>(throwableObject->jc), "message", "Ljava/lang/String;", throwableObject, 0)
    );
    detailedMsg = Converter::javastring2stdtring(messageField);
}
