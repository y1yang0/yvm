#ifndef YVM_EXCEPTION_H
#define YVM_EXCEPTION_H
#include <vector>
#include "JavaType.h"

class JavaClass;
class StackTrace {
public:
    void printStackTrace();
    void setThrowExceptionInfo(JObject * throwableObject);
    inline void extendExceptionStackTrace(const char * methodName) { exceptionStackTrace.push_back(methodName); }

protected:
    std::vector<const char *> exceptionStackTrace;

private:
    const JavaClass * throwExceptionClass = nullptr;
    std::string detailedMsg;
};

class JavaException : public StackTrace {
public:
    inline bool hasUnhandledException() const { return unhandledException; }
    inline void markExpception() { unhandledException = true; }
    inline void sweepException() { unhandledException = false; exceptionStackTrace.clear(); }

private:
    volatile bool unhandledException = false;
};


#endif
