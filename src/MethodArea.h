#ifndef YVM_METHODAREA_H
#define YVM_METHODAREA_H

#include "ClassFile.h"
#include <algorithm>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CodeExecution;
class JavaClass;
class ConcurrentGC;

class MethodArea {
	friend class ConcurrentGC;

public:
	MethodArea(const std::vector<std::string>& libPaths);
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

	std::unordered_set<const char*> linkedClasses;
	std::unordered_set<const char*> initedClasses;
	std::unordered_map<std::string, JavaClass*> classTable;
	std::vector<std::string> searchPaths;

	std::string parseNameToPath(const char* name);
};

#endif // YVM_METHODAREA_H
