#ifndef YVM_JAVACLASS_H
#define YVM_JAVACLASS_H

#include "ClassFile.h"
#include "FileReader.h"
#include "JavaType.h"
#include "MethodArea.h"
#include "Internal.h"
#include "YVM.h"

#define JAVA_9_MAJOR 53
#define JAVA_8_MAJOR 52
#define JAVA_7_MAJOR 51
#define JAVA_6_MAJOR 50

#define JAVA_CLASS_FILE_MAGIC_NUMBER 0XCAFEBABE

/**
 * \brief JavaClass is an in-memory representation of java class file. We should
 * call parseClassFile() to parse into proper structure before any operation on
 * JavaClass.
 */
class JavaClass {
    friend struct Inspector;
    friend struct YVM;
    friend class JavaHeap;
    friend class MethodArea;
    friend class CodeExecution;
    friend class ConcurrentGC;

public:
    explicit JavaClass(const std::string& classFilePath);
    ~JavaClass();
    JavaClass(const JavaClass& rhs) { this->raw = rhs.raw; }

public:
    const std::string getString(u2 index) const {
        return reinterpret_cast<const char*>(
            dynamic_cast<CONSTANT_Utf8*>(raw.constPoolInfo[index])->bytes);
    }

    const std::string getClassName() const {
        return getString(
            dynamic_cast<CONSTANT_Class*>(raw.constPoolInfo[raw.thisClass])
                ->nameIndex);
    }

    const std::string getSuperClassName() const {
        return raw.superClass == 0
                   ? ""
                   : getString(dynamic_cast<CONSTANT_Class*>(
                                   raw.constPoolInfo[raw.superClass])
                                   ->nameIndex);
    }

    bool hasSuperClass() const { return raw.superClass != 0; }

    void parseClassFile();

    std::vector<u2> getInterfacesIndex() const;
    MethodInfo* getMethod(const std::string& methodName,
                          const std::string& methodDescriptor) const;

private:
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

private:
    ClassFile raw{};
    FileReader reader;
    std::map<size_t, JType*> sfield;
};

#endif  // YVM_JAVACLASS_H
