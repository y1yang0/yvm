#ifndef YVM_JAVACLASS_H
#define YVM_JAVACLASS_H

#include "Type.h"
#include "JavaType.h"
#include "ClassFile.h"
#include "FileReader.h"
#include "MethodArea.h"
#include "YVM.h"

/**
 * \brief JavaClass is an in-memory representation of java class file. We should call
 * parseClassFile() to parse into proper structure before any operation on JavaClass.
 */
class JavaClass {
    friend struct Inspector;
    friend class YVM;
    friend class JavaHeap;
    friend class MethodArea;
    friend class CodeExecution;

public:
    explicit JavaClass(const char* classFilePath);
    ~JavaClass();
    JavaClass(const JavaClass& rhs) { this->raw = rhs.raw; }
    

public:
    inline decltype(auto) getString(u2 index) const {
        return reinterpret_cast<const char*>(dynamic_cast<CONSTANT_Utf8*>(raw.constPoolInfo[index])->bytes);
    }
    inline decltype(auto) getClassName() const {
        return getString(dynamic_cast<CONSTANT_Class*>(raw.constPoolInfo[raw.thisClass])->nameIndex);
    }
    inline decltype(auto) getSuperClassName() const {
        return raw.superClass == 0 ? nullptr : getString(dynamic_cast<CONSTANT_Class*>(raw.constPoolInfo[raw.superClass])->nameIndex);
    }
    inline bool hasSuperClass() const {
        return raw.superClass != 0;
    }
    
    void parseClassFile();
    
    std::vector<u2> getInterfacesIndex() const;
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
    ClassFile raw{};
    FileReader reader;
    std::map<size_t, JType*> sfield;
};


#endif //YVM_JAVACLASS_H
