#ifndef YVM_RAWCLASSFILE_H
#define YVM_RAWCLASSFILE_H

#include "Utils.h"
#include "Type.h"
#include "ConstantTag.h"

/****************************************************************************
 * ConstantPool definitions
 ****************************************************************************/
struct ConstantPoolInfo {
    virtual ~ConstantPoolInfo() = default;;
};

#define DEF_CONSTANT_WITH_2_FIELDS(name,type,field) \
struct CONSTANT_##name : public ConstantPoolInfo{   \
    static const u1 tag = ConstantTag::TAG_##name;    \
    type field; \
};


#define DEF_CONSTANT_WITH_3_FIELDS(name,type1,field1,type2,field2)   \
struct CONSTANT_##name : public ConstantPoolInfo{    \
    static  const u1 tag = ConstantTag::TAG_##name;   \
    type1 field1;                                           \
    type2 field2;   \
};
#define DEF_CONSTANT_WITH_4_FIELDS(name,type1,field1,type2,field2,type3,field3)   \
struct CONSTANT_##name : public ConstantPoolInfo{    \
    static  const u1 tag = ConstantTag::TAG_##name;   \
    type1 field1;   \
    type2 field2;   \
    type3 field3;   \
};

DEF_CONSTANT_WITH_2_FIELDS(Class,u2,nameIndex);

DEF_CONSTANT_WITH_3_FIELDS(Fieldref, u2, classIndex, u2, nameAndTypeIndex);

DEF_CONSTANT_WITH_3_FIELDS(Methodref, u2, classIndex, u2, nameAndTypeIndex);

DEF_CONSTANT_WITH_3_FIELDS(InterfaceMethodref,u2,classIndex,u2,nameAndTypeIndex);

DEF_CONSTANT_WITH_2_FIELDS(String,u2,stringIndex);

// Add extra *val* field to represent float value. We can use this value as long as we parsed it
// This could reduce redundant computation, so we add the same field to Integer/Float/Long/Double.
DEF_CONSTANT_WITH_3_FIELDS(Integer,u4,bytes,int32_t,val);

DEF_CONSTANT_WITH_3_FIELDS(Float,u4,bytes,float,val);

DEF_CONSTANT_WITH_4_FIELDS(Long,u4,highBytes,u4,lowBytes,int64_t,val);

DEF_CONSTANT_WITH_4_FIELDS(Double,u4,highBytes,u4,lowBytes,double,val);

DEF_CONSTANT_WITH_3_FIELDS(NameAndType,u2,nameIndex,u2,descriptorIndex);

// It's a special structure whose a field is an array of dynamic size
// We need override destructor functions to realse these memory while the object was destorying
struct CONSTANT_Utf8 : public ConstantPoolInfo {
    static const u1 tag = TAG_Utf8;
    u2 length;
    u1* bytes;

    ~CONSTANT_Utf8() override {
        delete[] bytes;
    }
};

DEF_CONSTANT_WITH_3_FIELDS(MethodHandle,u1,referenceKind,u2,referenceIndex);

DEF_CONSTANT_WITH_2_FIELDS(MethodType,u2,descriptorIndex);

DEF_CONSTANT_WITH_3_FIELDS(InvokeDynamic,u2,bootstrapMethodAttrIndex,u2,nameAndTypeIndex);

/****************************************************************************
 * Attributes definitions
 ****************************************************************************/
struct AttributeInfo {
    u2 attributeNameIndex;
    u4 attributeLength;
    virtual ~AttributeInfo() = default;
};

#define DEF_ATTR_START(name)  \
struct ATTR_##name : public AttributeInfo


DEF_ATTR_START(ConstantValue) {
    u2 constantValueIndex;
};

DEF_ATTR_START(Code) {
    u2 maxStack;
    u2 maxLocals;
    u4 codeLength;
    u1* code;
    u2 exceptionTableLength;

    struct _exceptionTable {
        u2 startPC;
        u2 endPC;
        u2 handlerPC;
        u2 catchType;
    }* exceptionTable;

    u2 attributeCount;
    AttributeInfo** attributes;

    ~ATTR_Code() override {
        delete[] code;
        delete[] exceptionTable;
        FOR_EACH(i,attributeCount) {
            delete attributes[i];
        }
        delete attributes;
    }
};

struct VerificationTypeInfo {
    virtual ~VerificationTypeInfo() = default;
};

#define DEF_VARIABLE_INFO_WITH_1_FIELDS(name)   \
struct VariableInfo_##name : public VerificationTypeInfo {    \
    static const u1 tag = VariableInfoTag::ITEM_##name;    \
};
#define DEF_VARIABLE_INFO_WITH_2_FIELDS(name,type,field)    \
struct VariableInfo_##name : public VerificationTypeInfo { \
    static const u1 tag = VariableInfoTag::ITEM_##name;    \
    type field; \
};

DEF_VARIABLE_INFO_WITH_1_FIELDS(Top);

DEF_VARIABLE_INFO_WITH_1_FIELDS(Integer);

DEF_VARIABLE_INFO_WITH_1_FIELDS(Float);

DEF_VARIABLE_INFO_WITH_1_FIELDS(Null);

DEF_VARIABLE_INFO_WITH_1_FIELDS(UninitializedThis);

DEF_VARIABLE_INFO_WITH_2_FIELDS(Object,u2,cpoolIndex);

DEF_VARIABLE_INFO_WITH_2_FIELDS(Uninitialized,u2,offset);

DEF_VARIABLE_INFO_WITH_1_FIELDS(Long);

DEF_VARIABLE_INFO_WITH_1_FIELDS(Double);

struct StackMapFrame {
    virtual ~StackMapFrame() = default;
};

#define DEF_FRAME_TYPE_WITH_1_FIELDS(name)  \
struct Frame_##name : public StackMapFrame{    \
    u2 frameType;   \
};
#define DEF_FRAME_TYPE_WITH_2_FIELDS(name,type,field)  \
struct Frame_##name : public StackMapFrame{\
    u2 frameType;   \
    type field;     \
};
#define DEF_FRAME_TYPE_WITH_3_FIELDS(name,type,field,type1,field1)  \
struct Frame_##name : public StackMapFrame{\
    u2 frameType;   \
    type field;     \
    type1 field1;   \
};
DEF_FRAME_TYPE_WITH_1_FIELDS(Same);

struct Frame_Same_locals_1_stack_item : public StackMapFrame {
    u2 frameType;
    VerificationTypeInfo** stack;

    ~Frame_Same_locals_1_stack_item() override {
        delete stack[0];
        delete stack;
    }
};

struct Frame_Same_locals_1_stack_item_extended : public StackMapFrame {
    u2 frameType;
    u2 offsetDelta;
    VerificationTypeInfo** stack;

    ~Frame_Same_locals_1_stack_item_extended() override {
        delete stack[0];
        delete stack;
    }
};

DEF_FRAME_TYPE_WITH_2_FIELDS(Chop,u2,offsetDelta);

DEF_FRAME_TYPE_WITH_2_FIELDS(Same_frame_extended,u2,offsetDelta);

struct Frame_Append : public StackMapFrame {
    u2 frameType;
    u2 offsetDelta;
    VerificationTypeInfo** stack;

    ~Frame_Append() override {
        FOR_EACH(i,frameType-251) {
            delete stack[i];
        }
        delete[] stack;
    }
};

struct Frame_Full : public StackMapFrame {
    u2 frameType;
    u2 offsetDelta;
    u2 numberOfLocals;
    VerificationTypeInfo** locals;
    u2 numberOfStackItems;
    VerificationTypeInfo** stack;

    ~Frame_Full() override {
        FOR_EACH(i,numberOfLocals) {
            delete locals[i];
        }
        delete[] locals;
        FOR_EACH(i,numberOfStackItems) {
            delete stack[i];
        }
        delete[] stack;
    }
};

DEF_ATTR_START(StackMapTable) {
    u2 numberOfEntries;
    StackMapFrame** entries;

    ~ATTR_StackMapTable() override {
        FOR_EACH(i,numberOfEntries) {
            delete entries[i];
        }
        delete[] entries;
    }
};

DEF_ATTR_START(Exception) {
    u2 numberOfExceptions;
    u2* exceptionIndexTable;

    ~ATTR_Exception() override {
        delete[] exceptionIndexTable;
    }
};

DEF_ATTR_START(InnerClasses) {
    u2 numberOfClasses;

    struct _classes {
        u2 innerClassInfoIndex;
        u2 outerClassInfoIndex;
        u2 innerNameIndex;
        u2 innerClassAccessFlags;
    }* classes;

    ~ATTR_InnerClasses() override {
        delete[] classes;
    }
};

DEF_ATTR_START(EnclosingMethod) {
    u2 classIndex;
    u2 methodIndex;
};

DEF_ATTR_START(Synthetic) {};

DEF_ATTR_START(Signature) {
    u2 signatureIndex;
};

DEF_ATTR_START(SourceFile) {
    u2 sourceFileIndex;
};

DEF_ATTR_START(SourceDebugExtension) {
    u1* debugExtension;

    ~ATTR_SourceDebugExtension() override {
        delete[] debugExtension;
    }
};

DEF_ATTR_START(LineNumberTable) {
    u2 lineNumberTableLength;

    struct _lineNumberTable {
        u2 startPC;
        u2 lineNumber;
    }* lineNumberTable;

    ~ATTR_LineNumberTable() override {
        delete[] lineNumberTable;
    }
};

DEF_ATTR_START(LocalVariableTable) {
    u2 localVariableTableLength;

    struct _localVariableTable {
        u2 startPC;
        u2 length;
        u2 nameIndex;
        u2 descriptorIndex;
        u2 index;
    }* localVariableTable;

    ~ATTR_LocalVariableTable() override {
        delete[] localVariableTable;
    }
};

DEF_ATTR_START(LocalVariableTypeTable) {
    u2 localVariableTypeTableLength;

    struct _localVariableTypeTable {
        u2 startPC;
        u2 length;
        u2 nameIndex;
        u2 signatureIndex;
        u2 index;
    }* localVariableTypeTable;

    ~ATTR_LocalVariableTypeTable() override {
        delete[] localVariableTypeTable;
    }
};

DEF_ATTR_START(Deprecated) {};

struct ElementValue {
    u1 tag;
    virtual ~ElementValue() = default;
};

struct Annotation {
    u2 typeIndex;
    u2 numElementValuePairs;

    struct _elementValuePairs {
        u2 elementNameIndex;
        ElementValue* value;

        ~_elementValuePairs() {
            delete value;
        }
    }* elementValuePairs;

    ~Annotation() {
        delete[] elementValuePairs;
    }
};


struct ElementValue_ConstantValueIndex : public ElementValue {
    u2 constValueIndex;
};

struct ElementValue_EnumConstValue : public ElementValue {
    u2 typeNameIndex;
    u2 constNameIndex;
};

struct ElementValue_ClassInfoIndex : public ElementValue {
    u2 classInfoIndex;
};

struct ElementValue_ArrayValue : public ElementValue {
    u2 numValues;
    ElementValue** values;

    ~ElementValue_ArrayValue() override {
        FOR_EACH(i, numValues) {
            delete values[i];
        }
        delete [] values;
    }
};

struct ElementValue_Annotation : public ElementValue {
    Annotation annotationValue;
};

DEF_ATTR_START(RuntimeVisibleAnnotations) {
    u2 numAnnotations;
    Annotation* annotations;

    ~ATTR_RuntimeVisibleAnnotations() override {
        delete[] annotations;
    }
};

DEF_ATTR_START(RuntimeInvisibleAnnotations) {
    u2 numAnnotations;
    Annotation* annotations;

    ~ATTR_RuntimeInvisibleAnnotations() override {
        delete[] annotations;
    }
};

DEF_ATTR_START(RuntimeVisibleParameterAnnotations) {
    u1 numParameters;

    struct _parameterAnnotations {
        u2 numAnnotations;
        Annotation* annotations;

        ~_parameterAnnotations() {
            delete[] annotations;
        }
    }* parameterAnnotations;

    ~ATTR_RuntimeVisibleParameterAnnotations() override {
        delete[] parameterAnnotations;
    }
};

DEF_ATTR_START(RuntimeInvisibleParameterAnnotations) {
    u1 numParameters;

    struct _parameterAnnotations {
        u2 numAnnotations;
        Annotation* annotations;

        ~_parameterAnnotations() {
            delete[] annotations;
        }
    }* parameterAnnotations;

    ~ATTR_RuntimeInvisibleParameterAnnotations() override {
        delete [] parameterAnnotations;
    }
};

struct TargetInfo {
    virtual ~TargetInfo() = default;
};

#define DEF_TARGET_WITH_1_FIELDS(name,type,field)  \
struct Target_##name : public TargetInfo{   \
    type field; \
};
#define DEF_TARGET_WITH_2_FIELDS(name,type,field,type1,field1)  \
struct Target_##name : public TargetInfo{   \
    type field; \
    type1 field1;   \
};

DEF_TARGET_WITH_1_FIELDS(TypeParameter,u1,typeParameterIndex);

DEF_TARGET_WITH_1_FIELDS(SuperType,u2,superTypeIndex);

DEF_TARGET_WITH_2_FIELDS(TypeParameterBound,u1,typeParameterIndex,u1,boundIndex);

struct Target_Empty : public TargetInfo {};

DEF_TARGET_WITH_1_FIELDS(FormalParameter,u1,formalParameter);

DEF_TARGET_WITH_1_FIELDS(Throws,u2,throwsTypeIndex);

struct Target_LocalVar : public TargetInfo {
    u2 tableLength;

    struct _table {
        u2 startPc;
        u2 length;
        u2 index;
    }* table;

    ~Target_LocalVar() override {
        delete [] table;
    }
};

DEF_TARGET_WITH_1_FIELDS(Catch,u2,exceptionTableIndex);

DEF_TARGET_WITH_1_FIELDS(Offset,u2,offset);

DEF_TARGET_WITH_2_FIELDS(TypeArgument,u2,offset,u1,typeArgumentIndex);


struct TypeAnnotation {
    u1 targetType;
    TargetInfo* targetInfo;

    struct TypePath {
        u1 pathLength;

        struct _path {
            u1 typePathKind;
            u1 typeArgumentIndex;
        }* path;

        ~TypePath() {
            delete [] path;
        }
    } targetPath;

    u2 typeIndex;
    u2 numElementValuePairs;

    struct _elementValuePairs {
        u2 elementNameIndex;
        ElementValue* value;

        ~_elementValuePairs() {
            delete value;
        }
    }* elementValuePairs;

    ~TypeAnnotation() {
        delete targetInfo;
        delete [] elementValuePairs;
    }
};

DEF_ATTR_START(RuntimeVisibleTypeAnnotations) {
    u2 numAnnotations;
    TypeAnnotation* annotations;

    ~ATTR_RuntimeVisibleTypeAnnotations() override {
        delete[] annotations;
    }
};

DEF_ATTR_START(RuntimeInvisibleTypeAnnotations) {
    u2 numAnnotations;
    TypeAnnotation* annotations;

    ~ATTR_RuntimeInvisibleTypeAnnotations() override {
        delete[] annotations;
    }
};

DEF_ATTR_START(AnnotationDefault) {
    ElementValue* defaultValue;

    ~ATTR_AnnotationDefault() {
        delete defaultValue;
    }
};

DEF_ATTR_START(BootstrapMethods) {
    u2 numBootstrapMethods;

    struct _bootstrapMethod {
        u2 bootstrapMethodRef;
        u2 numBootstrapArgument;
        u2* bootstrapArguments;

        ~_bootstrapMethod() {
            delete[] bootstrapArguments;
        }
    }* bootstrapMethod;

    ~ATTR_BootstrapMethods() override {
        delete[] bootstrapMethod;
    }
};

DEF_ATTR_START(MethodParameter) {
    u1 parameterCount;

    struct _parameters {
        u2 nameIndex;
        u2 accessFlags;
    }* parameters;

    ~ATTR_MethodParameter() override {
        delete[] parameters;
    }
};

/****************************************************************************
 * FieldInfo definition
 ****************************************************************************/
struct FieldInfo {
    u2 accessFlags;
    u2 nameIndex;
    u2 descriptorIndex;
    u2 attributeCount;
    AttributeInfo** attributes;

    ~FieldInfo() {
        FOR_EACH(i,attributeCount) {
            delete attributes[i];
        }
        if (attributeCount > 0) {
            delete[] attributes;
        }
    }
};

/****************************************************************************
 * MethodInfo definition
 ****************************************************************************/
struct MethodInfo {
    u2 accessFlags;
    u2 nameIndex;
    u2 descriptorIndex;
    u2 attributeCount;
    AttributeInfo** attributes;

    ~MethodInfo() {
        FOR_EACH(i,attributeCount) {
            delete attributes[i];
        }
        delete[] attributes;
    }
};

/****************************************************************************
 * Raw java class file format
 ****************************************************************************/
struct ClassFile {
    u4 magic;
    u2 minorVersion;
    u2 majorVersion;
    u2 constPoolCount;
    ConstantPoolInfo** constPoolInfo;
    u2 accessFlags;
    u2 thisClass;
    u2 superClass;
    u2 interfacesCount;
    u2* interfaces;
    u2 fieldsCount;
    FieldInfo* fields;
    u2 methodsCount;
    MethodInfo* methods;
    u2 attributesCount;
    AttributeInfo** attributes;

    ~ClassFile() {
        FOR_EACH(i,constPoolCount) {
            delete constPoolInfo[i];
        }
        delete[] constPoolInfo;

        if (interfacesCount > 0) {
            delete[] interfaces;
        }

        if (fieldsCount > 0) {
            delete[] fields;
        }

        if (methodsCount > 0) {
            delete[] methods;
        }

        FOR_EACH(i,attributesCount) {
            delete attributes[i];
        }
        delete[] attributes;
    }
};
#endif //YVM_RAWCLASSFILE_H
