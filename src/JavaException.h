#ifndef YVM_EXCEPTION_H
#define YVM_EXCEPTION_H
#include <string>
#include <vector>
#include "JavaType.h"

class JavaClass;

class StackTrace {
public:
    void printStackTrace();
    void setThrowExceptionInfo(JObject* throwableObject);
    void extendExceptionStackTrace(const std::string& methodName) {
        exceptionStackTrace.push_back(methodName);
    }

protected:
    std::vector<std::string> exceptionStackTrace;

private:
    const JavaClass* throwExceptionClass = nullptr;
    std::string detailedMsg;
};

class JavaException : public StackTrace {
public:
    bool hasUnhandledException() const { return unhandledException; }
    void markException() { unhandledException = true; }
    void sweepException() {
        unhandledException = false;
        exceptionStackTrace.clear();
    }

private:
    volatile bool unhandledException = false;
};

#endif
