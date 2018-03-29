#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "RuntimeEnv.h"
#include "CodeExecution.hpp"


extern RuntimeEnv yrt;

class YVM {
public:
    explicit YVM();

    bool loadClass(const char* name);
    bool linkClass(const char* name);
    bool initClass(const char* name);

    void callMain(const char * name);

    void registerNativeMethod(const char *className, const char * name, const char * descriptor, JType *(*func)(RuntimeEnv *env));
    void warmUp(const char  *libraryPath[], int howManySearchPath);

private:
    CodeExecution exec;
};


#endif //YVM_YVM_H
