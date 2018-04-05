#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "RuntimeEnv.h"
#include "CodeExecution.hpp"


extern RuntimeEnv yrt;

class YVM {
public:
    explicit YVM();

    static bool loadClass(const char* name);
    static bool linkClass(const char* name);
    bool initClass(CodeExecution& exec, const char* name);
    void callMain(const char* name);
    static void registerNativeMethod(const char* className, const char* name, const char* descriptor,
                                     JType*(*func)(RuntimeEnv* env));

    void warmUp(const char* libraryPath[], int howManySearchPath) const;

};


#endif //YVM_YVM_H
