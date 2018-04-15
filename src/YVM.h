#ifndef YVM_YVM_H
#define YVM_YVM_H

#include "RuntimeEnv.h"
#include "CodeExecution.hpp"
#include "Concurrent.hpp"

extern RuntimeEnv yrt;

struct YVM {
    explicit YVM();

    static bool loadClass(const char* name);
    static bool linkClass(const char* name);
    static bool initClass(CodeExecution& exec, const char* name);
    static void callMain(const char* name);
    static void warmUp(const std::vector<std::string> & libPaths);

    static ThreadPool executor;
};

#endif //YVM_YVM_H
