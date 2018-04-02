#ifndef YVM_EXCEPTION_H
#define YVM_EXCEPTION_H
#include <vector>

class JavaClass;

class StackTrace {
public:
    inline void setThrowExceptionType(const JavaClass * jc) { throwExceptionClass = jc; }
    inline void extendExceptionStackTrace(const char * methodName) { exceptionStackTrace.push_back(methodName); }
    inline void clearExceptionStackTrace() { exceptionStackTrace.clear(); }
    inline void setDetailedMessage(const std::string & str) { detailedMsg = str; }
    void printStackTrace();

private:
    std::vector<const char *> exceptionStackTrace;
    const JavaClass * throwExceptionClass = nullptr;
    std::string detailedMsg;
};

#endif
