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
    static bool initClass(CodeExecution& exec, const char* name);
    static void callMain(const char* name);
    static void warmUp(const std::vector<std::string> & libPaths);

};


#endif //YVM_YVM_H
