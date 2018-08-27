#ifndef YVM_METHODAREA_H
#define YVM_METHODAREA_H

#include <algorithm>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ClassFile.h"

class CodeExecution;
class JavaClass;
class ConcurrentGC;

class MethodArea {
    friend class ConcurrentGC;

public:
    MethodArea(const std::vector<std::string>& libPaths);
    ~MethodArea();

    JavaClass* findJavaClass(const std::string& jcName);
    bool loadJavaClass(const std::string& jcName);
    bool removeJavaClass(const std::string& jcName);
    void linkJavaClass(const std::string& jcName);
    void initJavaClass(CodeExecution& exec, const std::string& jcName);

public:
    JavaClass* loadClassIfAbsent(const std::string& jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        JavaClass* jc = findJavaClass(jcName);
        if (jc) {
            return jc;
        }
        loadJavaClass(jcName);
        return findJavaClass(jcName);
    }

    void linkClassIfAbsent(const std::string& jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        bool linked = false;
        for (auto p : linkedClasses) {
            if (p == jcName) {
                linked = true;
            }
        }
        if (!linked) {
            linkJavaClass(jcName);
        }
    }

    void initClassIfAbsent(CodeExecution& exec, const std::string& jcName) {
        std::lock_guard<std::recursive_mutex> lockMA(maMutex);

        bool inited = false;
        for (auto p : initedClasses) {
            if (p == jcName) {
                inited = true;
            }
        }
        if (!inited) {
            initJavaClass(exec, jcName);
        }
    }

private:
    std::recursive_mutex maMutex;

    std::unordered_set<std::string> linkedClasses;
    std::unordered_set<std::string> initedClasses;
    std::unordered_map<std::string, JavaClass*> classTable;
    std::vector<std::string> searchPaths;

    const std::string parseNameToPath(const std::string& name);
};

#endif  // YVM_METHODAREA_H
