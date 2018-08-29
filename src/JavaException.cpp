#include <cassert>
#include <cstdio>
#include "JavaClass.h"
#include "JavaException.h"

void StackTrace::printStackTrace() {
    assert(!exceptionStackTrace.empty());
    assert(throwExceptionClass != nullptr);

    printf("Thrown %s at %s()\n", throwExceptionClass->getClassName().c_str(),
           exceptionStackTrace[0].c_str());
    !detailedMsg.empty() ? printf("Reason:%s\n", detailedMsg.c_str())
                         : printf("");

    int deep = 0;
    for (size_t p = 1; p < exceptionStackTrace.size(); p++) {
        deep++;
        for (int i = 0; i < deep; i++) {
            printf("-");
        }
        printf("By its caller %s()\n", exceptionStackTrace[p].c_str());
    }
}

void StackTrace::setThrowExceptionInfo(JObject* throwableObject) {
    throwExceptionClass = throwableObject->jc;
    auto* messageField = dynamic_cast<JObject*>(yrt.jheap->getFieldByName(
        yrt.ma->findJavaClass("java/lang/Throwable"), "message",
        "Ljava/lang/String;", throwableObject));
    detailedMsg = Converter::javastring2stdtring(messageField);
}
