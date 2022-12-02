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

#include "ClassSpace.h"

#include "../classfile/AccessFlag.h"
#include "JavaClass.h"

using namespace std;

ClassSpace::ClassSpace(const string& path) {
    searchPaths.push_back(path);
}

ClassSpace::~ClassSpace() {
    for (auto& x : classTable) {
        delete x.second;
    }
}

JavaClass* ClassSpace::findJavaClass(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

    const auto pos = classTable.find(jcName);
    if (pos != classTable.end()) {
        return pos->second;
    }
    return nullptr;
}

bool ClassSpace::loadJavaClass(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

    auto path = parseNameToPath(jcName);

    if (path.length() != 0 && !findJavaClass(jcName)) {
        // Load this class which specified by jcName (it' a path string)
        auto* jc = new JavaClass(path);
        jc->parseClassFile();
        classTable.insert(make_pair(jc->getClassName(), jc));

        // Load super class if it doesn't exist in the class table
        if (!jc->getSuperClassName().empty() &&
            !findJavaClass(jc->getSuperClassName())) {
            this->loadJavaClass(jc->getSuperClassName());
        }

        // Load super interfaces if existed
        vector<u2>&& interfacesIdx = jc->getInterfacesIndex();
        if (jc->getInterfacesIndex().empty()) {
            for (auto idx : interfacesIdx) {
                this->loadJavaClass(jc->getString(idx));
            }
        }

        return true;
    }
    return false;
}

void ClassSpace::linkJavaClass(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

    JavaClass* javaClass = findJavaClass(jcName);
    assert(javaClass != NULL && "sanity check");
    FOR_EACH(fieldOffset, javaClass->raw.fieldsCount) {
        const string& descriptor = javaClass->getString(
            javaClass->raw.fields[fieldOffset].descriptorIndex);
        if (IS_FIELD_REF_CLASS(descriptor)) {
            // Special handling for field whose type is another class
            if (IS_FIELD_STATIC(
                    javaClass->raw.fields[fieldOffset].accessFlags)) {
                JObject* fieldObject = nullptr;

                FOR_EACH(fieldAttr,
                         javaClass->raw.fields[fieldOffset].attributeCount) {
                    if (typeid(*javaClass->raw.fields[fieldOffset]
                                    .attributes[fieldAttr]) ==
                        typeid(ATTR_ConstantValue)) {
                        if ("Ljava/lang/String;" == descriptor) {
                            const string& constantStr = javaClass->getString(
                                ((CONSTANT_String*)javaClass->raw.constPoolInfo
                                     [((ATTR_ConstantValue*)javaClass->raw
                                           .fields[fieldOffset]
                                           .attributes[fieldAttr])
                                          ->constantValueIndex])
                                    ->stringIndex);
                            size_t strLen = constantStr.length();
                            fieldObject = runtime.heap->createObject(
                                *loadClassIfAbsent("java/lang/String"));
                            fieldObject = runtime.heap->createObject(
                                *loadClassIfAbsent("java/lang/String"));
                            runtime.heap->putFieldByOffset(
                                *fieldObject, 0,
                                runtime.heap->createCharArray(constantStr,
                                                           strLen));
                        }
                    }
                }

                javaClass->staticVars.insert(
                    make_pair(fieldOffset, fieldObject));
            }
        } else if (IS_FIELD_REF_ARRAY(descriptor)) {
            // Special handling for field whose type is array. We create a null
            // JArray as a placeholder since we don't know more information
            // about size of array, so we defer to allocate memory while meeting
            // opcodes [newarray]/[multinewarray]

            if (IS_FIELD_STATIC(
                    javaClass->raw.fields[fieldOffset].accessFlags)) {
                JArray* uninitializedArray = nullptr;
                javaClass->staticVars.insert(
                    make_pair(fieldOffset, uninitializedArray));
            }
        } else {
            // Otherwise it's a basic type. We insert it into instance's field
            // or its class static field by determining its access flag
            if (IS_FIELD_STATIC(
                    javaClass->raw.fields[fieldOffset].accessFlags)) {
                JType* basicField = determineBasicType(descriptor);

                FOR_EACH(fieldAttr,
                         javaClass->raw.fields[fieldOffset].attributeCount) {
                    if (typeid(*javaClass->raw.fields[fieldOffset]
                                    .attributes[fieldAttr]) ==
                        typeid(ATTR_ConstantValue)) {
                        if (typeid(*javaClass->raw.constPoolInfo
                                        [((ATTR_ConstantValue*)javaClass->raw
                                              .fields[fieldOffset]
                                              .attributes[fieldAttr])
                                             ->constantValueIndex]) ==
                            typeid(CONSTANT_Long)) {
                            ((JLong*)basicField)->val =
                                ((CONSTANT_Long*)javaClass->raw.constPoolInfo
                                     [((ATTR_ConstantValue*)javaClass->raw
                                           .fields[fieldOffset]
                                           .attributes[fieldAttr])
                                          ->constantValueIndex])
                                    ->val;
                        } else if (typeid(*javaClass->raw.constPoolInfo
                                               [((ATTR_ConstantValue*)javaClass
                                                     ->raw.fields[fieldOffset]
                                                     .attributes[fieldAttr])
                                                    ->constantValueIndex]) ==
                                   typeid(CONSTANT_Double)) {
                            ((JDouble*)basicField)->val =
                                ((CONSTANT_Double*)javaClass->raw.constPoolInfo
                                     [((ATTR_ConstantValue*)javaClass->raw
                                           .fields[fieldOffset]
                                           .attributes[fieldAttr])
                                          ->constantValueIndex])
                                    ->val;
                        } else if (typeid(*javaClass->raw.constPoolInfo
                                               [((ATTR_ConstantValue*)javaClass
                                                     ->raw.fields[fieldOffset]
                                                     .attributes[fieldAttr])
                                                    ->constantValueIndex]) ==
                                   typeid(CONSTANT_Float)) {
                            ((JFloat*)basicField)->val =
                                ((CONSTANT_Float*)javaClass->raw.constPoolInfo
                                     [((ATTR_ConstantValue*)javaClass->raw
                                           .fields[fieldOffset]
                                           .attributes[fieldAttr])
                                          ->constantValueIndex])
                                    ->val;
                        } else if (typeid(*javaClass->raw.constPoolInfo
                                               [((ATTR_ConstantValue*)javaClass
                                                     ->raw.fields[fieldOffset]
                                                     .attributes[fieldAttr])
                                                    ->constantValueIndex]) ==
                                   typeid(CONSTANT_Integer)) {
                            ((JInt*)basicField)->val =
                                ((CONSTANT_Integer*)javaClass->raw.constPoolInfo
                                     [((ATTR_ConstantValue*)javaClass->raw
                                           .fields[fieldOffset]
                                           .attributes[fieldAttr])
                                          ->constantValueIndex])
                                    ->val;
                        } else {
                            SHOULD_NOT_REACH_HERE
                        }
                    }
                }

                javaClass->staticVars.insert(
                    make_pair(fieldOffset, basicField));
            }
        }
    }
    linkedClasses.insert(javaClass->getClassName());
}

void ClassSpace::initJavaClass(Interpreter& exec, const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);
    initedClasses.insert(jcName);
    auto* jc = findJavaClass(jcName);
    if (jc->findMethod("<clinit>", "()V")) {
        exec.invokeByName(jc, "<clinit>", "()V");
    }
}

JavaClass* ClassSpace::loadClassIfAbsent(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

    JavaClass* jc = findJavaClass(jcName);
    if (jc) {
        return jc;
    }
    loadJavaClass(jcName);
    return findJavaClass(jcName);
}

void ClassSpace::linkClassIfAbsent(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

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

void ClassSpace::initClassIfAbsent(Interpreter& exec, const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

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

bool ClassSpace::removeJavaClass(const string& jcName) {
    lock_guard<recursive_mutex> lockMA(maMutex);

    auto pos = classTable.find(jcName);
    if (pos != classTable.end()) {
        classTable.erase(pos);
        return true;
    }
    return false;
}

const string ClassSpace::parseNameToPath(const string& name) {
    // convert java.util.ArrayList to /jdk/xxx/java/util/ArrayList.class

    for (auto path : this->searchPaths) {
        if (path.length() > 0 && path[path.length() - 1] != '/') {
            path += '/';
        }
        path += name + ".class";
        fstream fin;
        fin.open(path, ios::in);
        if (fin) {
            fin.close();
            return path;  // Return only if it's a valid file path
        }
        fin.close();
    }
    return string("");
}
