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
    inline void clearExceptionStackTrace() { exceptionStackTrace.clear(); }
    std::string detailedMsg;

private:
    std::vector<const char *> exceptionStackTrace{};
    const JavaClass * throwExceptionClass = nullptr;
    
};

class JavaException : public StackTrace {
public:
    JavaException():StackTrace(){}
    inline bool hasUnhandledException() const { return unhandledException; }
    inline void markExpception() { unhandledException = true; }
    inline void sweepException() { unhandledException = false; clearExceptionStackTrace(); }


private:
    volatile bool unhandledException = false;
    StackTrace exceptionStackTrace;
};


#endif
