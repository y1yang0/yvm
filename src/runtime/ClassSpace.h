// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef YVM_CLASSSPACE_H
#define YVM_CLASSSPACE_H

#include <algorithm>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../classfile/ClassFile.h"

using namespace std;

class Interpreter;
class JavaClass;
class ConcurrentGC;

//--------------------------------------------------------------------------------
// Class space has responsible to manage all JavaClass objects. A complete
// lifecycle of java class consists of loading class into jvm, linking those
// loaded JavaClass which would initialize its static fields and finally
// initializing them. findJavaClass() used to check if there is a specific
// JavaClass existed in global class table.
//--------------------------------------------------------------------------------
class ClassSpace {
    friend class ConcurrentGC;

public:
    ClassSpace(const string& path);
    ~ClassSpace();

    JavaClass* findJavaClass(const string& jcName);
    bool loadJavaClass(const string& jcName);
    bool removeJavaClass(const string& jcName);
    void linkJavaClass(const string& jcName);
    void initJavaClass(Interpreter& exec, const string& jcName);

public:
    JavaClass* loadClassIfAbsent(const string& jcName);
    void linkClassIfAbsent(const string& jcName);
    void initClassIfAbsent(Interpreter& exec, const string& jcName);

private:
    const string parseNameToPath(const string& name);

private:
    recursive_mutex maMutex;

    unordered_set<string> linkedClasses;
    unordered_set<string> initedClasses;
    unordered_map<string, JavaClass*> classTable;

    vector<string> searchPaths;
};

#endif  // YVM_CLASSSPACE_H
