#ifndef YVM_METHODAREA_H
#define YVM_METHODAREA_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
class CodeExecution;
class JavaClass;

enum NameReprestation {
    DOT_QUALIFIED_NAME,
    SLASH_QUALIFIED_NAME,
    PURE_FILE_NAME
};

class MethodArea {
public:
    MethodArea(const char *searchPath[], int howManySearchPath);
    ~MethodArea();

    JavaClass* findJavaClass(const char* jcName);
    bool loadJavaClass(const char* jcName);
    bool removeJavaClass(const char* jcName);
    void linkJavaClass(const char* jcName);
    void initJavaClass(CodeExecution& exec, const char* jcName);

public:
    inline JavaClass* loadClassIfAbsent(const char* jcName) {
        JavaClass* jc = findJavaClass(jcName);
        if (jc) {
            return jc;
        }
        else {
            loadJavaClass(jcName);
            return findJavaClass(jcName);
        }
    }

    inline void linkClassIfAbsent(const char* jcName) {
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

    inline void initClassIfAbsent(CodeExecution& exec, const char* jcName) {
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
    std::vector<const char *> linkedClasses;
    std::vector<const char *> initedClasses;
    std::map<std::string, JavaClass *> classTable;
    std::vector<std::string> searchPaths;

    std::string parseNameToPath(const char* name);
};

#endif  //YVM_METHODAREA_H
