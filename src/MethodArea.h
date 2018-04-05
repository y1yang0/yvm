#ifndef YVM_METHODAREA_H
#define YVM_METHODAREA_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
#include "ClassFile.h"
#include <mutex>
class CodeExecution;
class JavaClass;

class MethodArea {
public:
    MethodArea(const std::vector<std::string> & libPaths);
    ~MethodArea();

    JavaClass* findJavaClass(const char* jcName);
    bool loadJavaClass(const char* jcName);
    bool removeJavaClass(const char* jcName);
    void linkJavaClass(const char* jcName);
    void initJavaClass(CodeExecution& exec, const char* jcName);

public:
    JavaClass* loadClassIfAbsent(const char* jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        JavaClass* jc = findJavaClass(jcName);
        if (jc) {
            return jc;
        }
        loadJavaClass(jcName);
        return findJavaClass(jcName);
    }

    void linkClassIfAbsent(const char* jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        bool linked = false;
        for (auto* p : linkedClasses) {
            if (strcmp(p, jcName) == 0) {
                linked = true;
            }
        }
        if (!linked) {
            linkJavaClass(jcName);
        }
    }

    void initClassIfAbsent(CodeExecution& exec, const char* jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        bool inited = false;
        for (auto* p : initedClasses) {
            if (strcmp(p, jcName) == 0) {
                inited = true;
            }
        }
        if (!inited) {
            initJavaClass(exec, jcName);
        }
    }

private:
    std::recursive_mutex maMutex;

    std::vector<const char *> linkedClasses;
    std::vector<const char *> initedClasses;
    std::map<std::string, JavaClass *> classTable;
    std::vector<std::string> searchPaths;

    std::string parseNameToPath(const char* name);
};

#endif  //YVM_METHODAREA_H
