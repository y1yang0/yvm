#ifndef YVM_JAVACLASS_H
#define YVM_JAVACLASS_H

#include "../classfile/ClassFile.h"
#include "../classfile/FileReader.h"
#include "../interpreter/Internal.h"
#include "../misc/Utils.h"
#include "../vm/YVM.h"
#include "JavaType.h"
#include "MethodArea.h"

#define JAVA_9_MAJOR 53
#define JAVA_8_MAJOR 52
#define JAVA_7_MAJOR 51
#define JAVA_6_MAJOR 50

#define JAVA_CLASS_FILE_MAGIC_NUMBER 0XCAFEBABE

using namespace std;

//--------------------------------------------------------------------------------
// JavaClass is an in-memory representation of java class file. We should call
// parseClassFile() to parse into proper structure before any operation on*
// JavaClass.
//--------------------------------------------------------------------------------
class JavaClass {
    friend struct Inspector;
    friend struct YVM;
    friend class JavaHeap;
    friend class MethodArea;
    friend class Interpreter;
    friend class ConcurrentGC;

public:
    explicit JavaClass(const string& classFilePath);
    ~JavaClass();
    JavaClass(const JavaClass& rhs);

public:
    forceinline ConstantPoolInfo* getConstPoolItem(u2 index) const {
        return raw.constPoolInfo[index];
    }

    forceinline const string getString(u2 index) const {
        return reinterpret_cast<const char*>(
            dynamic_cast<CONSTANT_Utf8*>(raw.constPoolInfo[index])->bytes);
    }

    forceinline const string getClassName() const {
        return getString(
            dynamic_cast<CONSTANT_Class*>(raw.constPoolInfo[raw.thisClass])
                ->nameIndex);
    }

    forceinline const string getSuperClassName() const {
        return raw.superClass == 0
                   ? ""
                   : getString(dynamic_cast<CONSTANT_Class*>(
                                   raw.constPoolInfo[raw.superClass])
                                   ->nameIndex);
    }

    forceinline const string getInterfaceClassName(u2 index) const {
        return getString(dynamic_cast<CONSTANT_Class*>(
                             raw.constPoolInfo[raw.interfaces[index]])
                             ->nameIndex);
    }

    forceinline bool hasSuperClass() const { return raw.superClass != 0; }

    forceinline bool hasInterface() const { return raw.interfacesCount != 0; }

    forceinline u2 getInterfaceCount() const { return raw.interfacesCount; }

    forceinline u2 getAccessFlag() const { return raw.accessFlags; }

public:
    MethodInfo* findMethod(const string& methodName,
                           const string& methodDescriptor) const;
    bool setStaticVar(const string& name, const string& descriptor,
                      JType* value);
    JType* getStaticVar(const string& name, const string& descriptor);

private:
    void parseClassFile();
    bool parseConstantPool(u2 cpCount);
    bool parseInterface(u2 interfaceCount);
    bool parseField(u2 fieldCount);
    bool parseMethod(u2 methodCount);
    bool parseAttribute(AttributeInfo**(&attrs), u2 attributeCount);

private:
    VerificationTypeInfo* determineVerificationType(u1 tag);
    TargetInfo* determineTargetType(u1 tag);
    ElementValue* readToElementValueStructure();
    Annotation readToAnnotationStructure();
    vector<u2> getInterfacesIndex() const;

private:
    ClassFile raw{};
    FileReader reader;
    map<size_t, JType*> staticVars;
};

#endif  // YVM_JAVACLASS_H
