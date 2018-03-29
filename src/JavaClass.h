#ifndef YVM_JAVACLASS_H
#define YVM_JAVACLASS_H

#include "Type.h"
#include "JavaType.h"
#include "ClassFile.h"
#include "FileReader.h"
#include "MethodArea.h"
#include "YVM.h"

class JavaClass {
    friend struct Inspector;
    friend class YVM;
    friend class JavaHeap;
    friend class MethodArea;
    friend class CodeExecution;

public:
    explicit JavaClass(const char* classFilePath);
    JavaClass(const JavaClass& rhs);
    ~JavaClass();

public:
    u1* getClassName();
    u1* getSuperClassName() const;
    void parseClassFile();
    u1* getString(u2 index) const;
    std::vector<u2> getInterfacesIndex();
    MethodInfo* getMethod(const char* methodName, const char* methodDescriptor) const;

private:
    bool parseConstantPool(u2 cpCount);
    bool parseInterface(u2 interfaceCount);
    bool parseField(u2 fieldCount);
    bool parseMethod(u2 methodCount);
    bool parseAttribute(AttributeInfo** (&attrs), u2 attributeCount);

private:
    VerificationTypeInfo* determineVerificationType(u1 tag);
    TargetInfo* determineTargetType(u1 tag);
    ElementValue* readToElementValueStructure();
    Annotation readToAnnotationStructure();

private:
    ClassFile raw;
    FileReader reader;
    std::map<size_t, JType*> sfield;
};


#endif //YVM_JAVACLASS_H
