#include <cassert>
#include <iostream>
#include <vector>
#include "../misc/Debug.h"
#include "../vm/YVM.h"
#include "JavaClass.h"
#include "MethodArea.h"

#pragma warning(disable : 4715)

using namespace std;

JavaClass::JavaClass(const string& classFilePath) : reader(classFilePath) {
    raw.constPoolInfo = nullptr;
    raw.fields = nullptr;
    raw.methods = nullptr;
    raw.attributes = nullptr;
}

JavaClass::~JavaClass() {
    for (auto& i : sfield) {
        delete i.second;
    }
}

vector<u2> JavaClass::getInterfacesIndex() const {
    if (raw.interfacesCount == 0) return vector<u2>();
    vector<u2> v;
    FOR_EACH(i, raw.interfacesCount) {
        v.push_back(
            dynamic_cast<CONSTANT_Class*>(raw.constPoolInfo[raw.interfaces[i]])
                ->nameIndex);
    }
    return v;
}

MethodInfo* JavaClass::findMethod(const string& methodName,
                                  const string& methodDescriptor) const {
    FOR_EACH(i, raw.methodsCount) {
        assert(typeid(*raw.constPoolInfo[raw.methods[i].nameIndex]) ==
               typeid(CONSTANT_Utf8));

        const string& mn = getString(raw.methods[i].nameIndex);
        const string& md = getString(raw.methods[i].descriptorIndex);
        if (mn == methodName && md == methodDescriptor) {
            return &raw.methods[i];
        }
    }
    return nullptr;
}

void JavaClass::parseClassFile() {
    int ff = 0;
    raw.magic = reader.readget4();
    if (raw.magic != JAVA_CLASS_FILE_MAGIC_NUMBER) {
        goto error;
    }
    raw.minorVersion = reader.readget2();
    raw.majorVersion = reader.readget2();
#ifdef YVM_DEBUG_SHOW_VERSION
    Inspector::printClassFileVersion(*this);
#endif
    if (raw.majorVersion < JAVA_6_MAJOR || raw.majorVersion > JAVA_9_MAJOR) {
        goto error;
    }
    raw.constPoolCount = reader.readget2();
    if (raw.constPoolCount > 0 && !parseConstantPool(raw.constPoolCount)) {
        cerr << __func__ << ":Failed to parse constant pool\n";
        exit(EXIT_FAILURE);
    }
#ifdef YVM_DEBUG_SHOW_CONSTANT_POOL_TABLE
    Inspector::printConstantPool(*this);
#endif

    raw.accessFlags = reader.readget2();
#ifdef YVM_DEBUG_SHOW_CLASS_ACCESS_FLAGS
    Inspector::printClassAccessFlag(*this);
#endif

    raw.thisClass = reader.readget2();
    raw.superClass = reader.readget2();
    raw.interfacesCount = reader.readget2();
    if (raw.interfacesCount > 0 && !parseInterface(raw.interfacesCount)) {
        cerr << __func__ << ":Failed to parse interfaces\n";
        exit(EXIT_FAILURE);
    }
#ifdef YVM_DEBUG_SHOW_INTERFACE
    Inspector::printInterfaces(*this);
#endif

    raw.fieldsCount = reader.readget2();
    if (raw.fieldsCount > 0 && !parseField(raw.fieldsCount)) {
        cerr << __func__ << ":Failed to parse fields\n";
        exit(EXIT_FAILURE);
    }
#ifdef YVM_DEBUG_SHOW_CLASS_FIELD
    Inspector::printField(*this);
#endif

    raw.methodsCount = reader.readget2();
    if (raw.methodsCount > 0 && !parseMethod(raw.methodsCount)) {
        cerr << __func__ << ":Failed to parse methods\n";
        exit(EXIT_FAILURE);
    }
#ifdef YVM_DEBUG_SHOW_CLASS_METHOD
    Inspector::printMethod(*this);
#endif

    raw.attributesCount = reader.readget2();
    if (raw.attributesCount > 0 &&
        !parseAttribute(raw.attributes, raw.attributesCount)) {
        cerr << __func__ << ":Failed to parse class file's attributes\n";
        exit(EXIT_FAILURE);
    }
#ifdef YVM_DEBUG_SHOW_CLASS_ATTRIBUTE
    Inspector::printClassFileAttrs(*this);
#endif

    if (!reader.haveNoExtraBytes()) {
        cerr << __func__ << ":Extra bytes existed in class file\n";
        exit(EXIT_FAILURE);
    }

    return;
error:
    cerr << __func__ << ":Failed to read content from bytecode file \n";
    exit(EXIT_FAILURE);
}

bool JavaClass::parseConstantPool(u2 cpCount) {
    raw.constPoolInfo = new ConstantPoolInfo*[cpCount];
    raw.constPoolInfo[0] = nullptr;

    if (!raw.constPoolInfo) {
        cerr << "Can not allocate memory to load class file\n";
        return false;
    }

    // As JVM 8 specification described, the index of constant pool
    // started from 1 to constant_pool_count-1
    for (int i = 1; i <= cpCount - 1; i++) {
        u1 tag = reader.readget1();
        ConstantPoolInfo* slot;
        switch (tag) {
            case TAG_Class: {
                slot = new CONSTANT_Class();
                dynamic_cast<CONSTANT_Class*>(slot)->nameIndex =
                    reader.readget2();
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Class*>(slot);
                break;
            }
            case TAG_Fieldref: {
                slot = new CONSTANT_Fieldref();
                dynamic_cast<CONSTANT_Fieldref*>(slot)->classIndex =
                    reader.readget2();
                dynamic_cast<CONSTANT_Fieldref*>(slot)->nameAndTypeIndex =
                    reader.readget2();
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Fieldref*>(slot);
                break;
            }

            case TAG_Methodref: {
                slot = new CONSTANT_Methodref();
                dynamic_cast<CONSTANT_Methodref*>(slot)->classIndex =
                    reader.readget2();
                dynamic_cast<CONSTANT_Methodref*>(slot)->nameAndTypeIndex =
                    reader.readget2();
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Methodref*>(slot);
                break;
            }

            case TAG_InterfaceMethodref: {
                slot = new CONSTANT_InterfaceMethodref();
                dynamic_cast<CONSTANT_InterfaceMethodref*>(slot)->classIndex =
                    reader.readget2();
                dynamic_cast<CONSTANT_InterfaceMethodref*>(slot)
                    ->nameAndTypeIndex = reader.readget2();
                raw.constPoolInfo[i] =
                    dynamic_cast<CONSTANT_InterfaceMethodref*>(slot);
                break;
            }

            case TAG_String: {
                slot = new CONSTANT_String();
                dynamic_cast<CONSTANT_String*>(slot)->stringIndex =
                    reader.readget2();
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_String*>(slot);
                break;
            }
            case TAG_Integer: {
                slot = new CONSTANT_Integer();
                dynamic_cast<CONSTANT_Integer*>(slot)->bytes =
                    reader.readget4();
                dynamic_cast<CONSTANT_Integer*>(slot)->val =
                    dynamic_cast<CONSTANT_Integer*>(slot)->bytes;

                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Integer*>(slot);
                break;
            }
            case TAG_Float: {
                slot = new CONSTANT_Float();
                dynamic_cast<CONSTANT_Float*>(slot)->bytes = reader.readget4();

                dynamic_cast<CONSTANT_Float*>(slot)->val =
                    *(float*)(&dynamic_cast<CONSTANT_Float*>(slot)->bytes);
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Float*>(slot);
                break;
            }
            case TAG_Long: {
                slot = new CONSTANT_Long();
                dynamic_cast<CONSTANT_Long*>(slot)->highBytes =
                    reader.readget4();
                dynamic_cast<CONSTANT_Long*>(slot)->lowBytes =
                    reader.readget4();

                dynamic_cast<CONSTANT_Long*>(slot)->val =
                    (((int64_t) dynamic_cast<CONSTANT_Long*>(slot)->highBytes)
                     << 32) +
                    dynamic_cast<CONSTANT_Long*>(slot)->lowBytes;
                raw.constPoolInfo[i++] = dynamic_cast<CONSTANT_Long*>(slot);
                // All 8-byte constants take up two slot in the constant_pool
                // table
                raw.constPoolInfo[i] = nullptr;
                break;
            }
            case TAG_Double: {
                slot = new CONSTANT_Double();
                dynamic_cast<CONSTANT_Double*>(slot)->highBytes =
                    reader.readget4();
                dynamic_cast<CONSTANT_Double*>(slot)->lowBytes =
                    reader.readget4();

                int64_t val =
                    (((int64_t) dynamic_cast<CONSTANT_Double*>(slot)->highBytes)
                     << 32) +
                    dynamic_cast<CONSTANT_Double*>(slot)->lowBytes;
                dynamic_cast<CONSTANT_Double*>(slot)->val = *(double*)&val;
                raw.constPoolInfo[i++] = dynamic_cast<CONSTANT_Double*>(slot);
                // All 8-byte constants take up two slot in the constant_pool
                // table
                raw.constPoolInfo[i] = nullptr;
                break;
            }
            case TAG_NameAndType: {
                slot = new CONSTANT_NameAndType();
                dynamic_cast<CONSTANT_NameAndType*>(slot)->nameIndex =
                    reader.readget2();
                dynamic_cast<CONSTANT_NameAndType*>(slot)->descriptorIndex =
                    reader.readget2();
                raw.constPoolInfo[i] =
                    dynamic_cast<CONSTANT_NameAndType*>(slot);
                break;
            }
            case TAG_Utf8: {
                slot = new CONSTANT_Utf8();
                u2 len = reader.readget2();
                dynamic_cast<CONSTANT_Utf8*>(slot)->length = len;
                dynamic_cast<CONSTANT_Utf8*>(slot)->bytes = new u1[len + 1];
                // The utf8 string is not end with '\0' since we do not need to
                // reserve extra 1 byte
                for (int k = 0; k < len; k++) {
                    dynamic_cast<CONSTANT_Utf8*>(slot)->bytes[k] =
                        reader.readget1();
                }
                dynamic_cast<CONSTANT_Utf8*>(slot)->bytes[len] =
                    '\0';  // End with '\0' for simplicity

                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_Utf8*>(slot);
                // Todo: support unicode string ; here we just add null-char at
                // the end of char array
                break;
            }
            case TAG_MethodHandle: {
                slot = new CONSTANT_MethodHandle();
                dynamic_cast<CONSTANT_MethodHandle*>(slot)->referenceKind =
                    reader.readget1();
                dynamic_cast<CONSTANT_MethodHandle*>(slot)->referenceIndex =
                    reader.readget2();
                raw.constPoolInfo[i] =
                    dynamic_cast<CONSTANT_MethodHandle*>(slot);
                break;
            }
            case TAG_MethodType: {
                slot = new CONSTANT_MethodType();
                dynamic_cast<CONSTANT_MethodType*>(slot)->descriptorIndex =
                    reader.readget2();
                raw.constPoolInfo[i] = dynamic_cast<CONSTANT_MethodType*>(slot);
                break;
            }
            case TAG_InvokeDynamic: {
                slot = new CONSTANT_InvokeDynamic();
                dynamic_cast<CONSTANT_InvokeDynamic*>(slot)
                    ->bootstrapMethodAttrIndex = reader.readget2();
                dynamic_cast<CONSTANT_InvokeDynamic*>(slot)->nameAndTypeIndex =
                    reader.readget2();
                raw.constPoolInfo[i] =
                    dynamic_cast<CONSTANT_InvokeDynamic*>(slot);
                break;
            }
            default:
                cerr << "undefined constant pool type\n";
                return false;
        }
    }
    return true;
}

bool JavaClass::parseInterface(u2 interfaceCount) {
    raw.interfaces = new u2[interfaceCount];
    FOR_EACH(i, interfaceCount) {
        raw.interfaces[i] = reader.readget2();
        // Each index must be a valid constant pool subscript, which pointed to
        // a CONSTANT_Class structure
        assert(typeid(*raw.constPoolInfo[raw.interfaces[i]]) ==
               typeid(CONSTANT_Class));
    }

    return true;
}

bool JavaClass::parseField(u2 fieldCount) {
    raw.fields = new FieldInfo[fieldCount];
    if (!raw.fields) {
        cerr << __func__ << ":Can not allocate memory to load class file\n";
        return false;
    }

    FOR_EACH(i, fieldCount) {
        raw.fields[i].accessFlags = reader.readget2();
        raw.fields[i].nameIndex = reader.readget2();
        raw.fields[i].descriptorIndex = reader.readget2();
        raw.fields[i].attributeCount = reader.readget2();
        parseAttribute(raw.fields[i].attributes, raw.fields[i].attributeCount);
    }
    return true;
}

bool JavaClass::parseMethod(u2 methodCount) {
    raw.methods = new MethodInfo[methodCount];
    if (!raw.methods) {
        cerr << __func__ << ":Can not allocate memory to load class file\n";
        return false;
    }

    FOR_EACH(i, raw.methodsCount) {
        raw.methods[i].accessFlags = reader.readget2();
        raw.methods[i].nameIndex = reader.readget2();
        raw.methods[i].descriptorIndex = reader.readget2();
        raw.methods[i].attributeCount = reader.readget2();
        parseAttribute(raw.methods[i].attributes,
                       raw.methods[i].attributeCount);
    }
    return true;
}

bool JavaClass::parseAttribute(AttributeInfo**(&attrs), u2 attributeCount) {
    attrs = new AttributeInfo*[attributeCount];

    if (attrs == nullptr) {
        cerr << __func__ << ":Can not allocate memory to load class file\n";
        return false;
    }

    for (decltype(attributeCount) i = 0; i < attributeCount; i++) {
        const u2 attrStrIndex = reader.readget2();

        if (typeid(*raw.constPoolInfo[attrStrIndex]) != typeid(CONSTANT_Utf8)) {
            return false;
        }

        char* attrName =
            (char*)dynamic_cast<CONSTANT_Utf8*>(raw.constPoolInfo[attrStrIndex])
                ->bytes;
        IS_ATTR_ConstantValue(attrName) {
            auto* attr = new ATTR_ConstantValue;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->constantValueIndex = reader.readget2();

            attrs[i] = attr;
            continue;
        }
        IS_ATTR_Code(attrName) {
            auto* attr = new ATTR_Code;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->maxStack = reader.readget2();
            attr->maxLocals = reader.readget2();
            attr->codeLength = reader.readget4();

            attr->code = new u1[attr->codeLength];
            FOR_EACH(k, attr->codeLength) { attr->code[k] = reader.readget1(); }

            attr->exceptionTableLength = reader.readget2();
            attr->exceptionTable =
                new ATTR_Code::ExceptionTable[attr->exceptionTableLength];
            FOR_EACH(k, attr->exceptionTableLength) {
                attr->exceptionTable[k].startPC = reader.readget2();
                attr->exceptionTable[k].endPC = reader.readget2();
                attr->exceptionTable[k].handlerPC = reader.readget2();
                attr->exceptionTable[k].catchType = reader.readget2();
            }

            attr->attributeCount = reader.readget2();
            parseAttribute(attr->attributes, attr->attributeCount);

            attrs[i] = attr;
            continue;
        }
        IS_ATTR_StackMapTable(attrName) {
            auto* attr = new ATTR_StackMapTable;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numberOfEntries = reader.readget2();
            attr->entries = new StackMapFrame*[attr->numberOfEntries];
            FOR_EACH(k, attr->numberOfEntries) {
                u1 frameType = reader.readget1();
                if (IS_STACKFRAME_same_frame(frameType)) {
                    auto* frame = new Frame_Same();
                    attr->entries[k] = frame;
                } else if (IS_STACKFRAME_same_locals_1_stack_item_frame(
                               frameType)) {
                    auto* frame = new Frame_Same_locals_1_stack_item;
                    frame->stack = new VerificationTypeInfo*[1];
                    frame->stack[0] =
                        determineVerificationType(reader.readget1());
                    attr->entries[k] = frame;
                } else if (
                    IS_STACKFRAME_same_locals_1_stack_item_frame_extended(
                        frameType)) {
                    auto* frame = new Frame_Same_locals_1_stack_item_extended;
                    frame->offsetDelta = reader.readget2();
                    frame->stack = new VerificationTypeInfo*[1];
                    frame->stack[0] =
                        determineVerificationType(reader.readget1());
                    attr->entries[k] = frame;
                } else if (IS_STACKFRAME_chop_frame(frameType)) {
                    auto* frame = new Frame_Chop;
                    frame->offsetDelta = reader.readget2();
                    attr->entries[k] = frame;
                } else if (IS_STACKFRAME_same_frame_extended(frameType)) {
                    auto* frame = new Frame_Same_frame_extended;
                    frame->offsetDelta = reader.readget2();
                    attr->entries[k] = frame;
                } else if (IS_STACKFRAME_append_frame(frameType)) {
                    auto* frame = new Frame_Append;
                    frame->frameType = frameType;
                    // It's important to store current frame type since
                    // ~Frame_Append need it to release memory
                    frame->offsetDelta = reader.readget2();
                    frame->stack = new VerificationTypeInfo*[frameType - 251];
                    FOR_EACH(p, frameType - 251) {
                        frame->stack[p] =
                            determineVerificationType(reader.readget1());
                    }
                    attr->entries[k] = frame;
                } else if (IS_STACKFRAME_full_frame(frameType)) {
                    auto* frame = new Frame_Full;
                    frame->offsetDelta = reader.readget2();
                    frame->numberOfLocals = reader.readget2();
                    frame->locals =
                        new VerificationTypeInfo*[frame->numberOfLocals];
                    FOR_EACH(p, frame->numberOfLocals) {
                        frame->locals[p] =
                            determineVerificationType(reader.readget1());
                    }
                    frame->numberOfStackItems = reader.readget2();
                    frame->stack =
                        new VerificationTypeInfo*[frame->numberOfStackItems];
                    FOR_EACH(p, frame->numberOfStackItems) {
                        frame->stack[p] =
                            determineVerificationType(reader.readget1());
                    }
                    attr->entries[k] = frame;
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_Exceptions(attrName) {
            auto* attr = new ATTR_Exception;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numberOfExceptions = reader.readget2();
            attr->exceptionIndexTable = new u2[attr->numberOfExceptions];
            FOR_EACH(k, attr->numberOfExceptions) {
                attr->exceptionIndexTable[k] = reader.readget2();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_InnerClasses(attrName) {
            auto* attr = new ATTR_InnerClasses;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numberOfClasses = reader.readget2();
            attr->classes =
                new ATTR_InnerClasses::_classes[attr->numberOfClasses];
            FOR_EACH(k, attr->numberOfClasses) {
                attr->classes[k].innerClassInfoIndex = reader.readget2();
                attr->classes[k].outerClassInfoIndex = reader.readget2();
                attr->classes[k].innerNameIndex = reader.readget2();
                attr->classes[k].innerClassAccessFlags = reader.readget2();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_EnclosingMethod(attrName) {
            auto* attr = new ATTR_EnclosingMethod;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->classIndex = reader.readget2();
            attr->methodIndex = reader.readget2();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_Synthetic(attrName) {
            auto* attr = new ATTR_Synthetic;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_Signature(attrName) {
            auto* attr = new ATTR_Signature;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->signatureIndex = reader.readget2();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_SourceFile(attrName) {
            auto* attr = new ATTR_SourceFile;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->sourceFileIndex = reader.readget2();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_SourceDebugExtension(attrName) {
            auto* attr = new ATTR_SourceDebugExtension;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->debugExtension = new u1[attr->attributeLength];
            FOR_EACH(k, attr->attributeLength) {
                attr->debugExtension[k] = reader.readget1();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_LineNumberTable(attrName) {
            auto* attr = new ATTR_LineNumberTable;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->lineNumberTableLength = reader.readget2();
            attr->lineNumberTable = new ATTR_LineNumberTable::_lineNumberTable
                [attr->lineNumberTableLength];
            FOR_EACH(k, attr->lineNumberTableLength) {
                attr->lineNumberTable[k].startPC = reader.readget2();
                attr->lineNumberTable[k].lineNumber = reader.readget2();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_LocalVariableTable(attrName) {
            auto* attr = new ATTR_LocalVariableTable;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->localVariableTableLength = reader.readget2();
            attr->localVariableTable = new ATTR_LocalVariableTable::
                _localVariableTable[attr->localVariableTableLength];
            FOR_EACH(k, attr->localVariableTableLength) {
                attr->localVariableTable[k].startPC = reader.readget2();
                attr->localVariableTable[k].length = reader.readget2();
                attr->localVariableTable[k].nameIndex = reader.readget2();
                attr->localVariableTable[k].descriptorIndex = reader.readget2();
                attr->localVariableTable[k].index = reader.readget2();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_LocalVariableTypeTable(attrName) {
            auto* attr = new ATTR_LocalVariableTypeTable;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->localVariableTypeTableLength = reader.readget2();
            attr->localVariableTypeTable = new ATTR_LocalVariableTypeTable::
                _localVariableTypeTable[attr->localVariableTypeTableLength];
            FOR_EACH(k, attr->localVariableTypeTableLength) {
                attr->localVariableTypeTable[k].startPC = reader.readget2();
                attr->localVariableTypeTable[k].length = reader.readget2();
                attr->localVariableTypeTable[k].nameIndex = reader.readget2();
                attr->localVariableTypeTable[k].signatureIndex =
                    reader.readget2();
                attr->localVariableTypeTable[k].index = reader.readget2();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_Deprecated(attrName) {
            auto* attr = new ATTR_Deprecated;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeVisibleAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeVisibleAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numAnnotations = reader.readget2();
            attr->annotations = new Annotation[attr->numAnnotations];
            FOR_EACH(k, attr->numAnnotations) {
                attr->annotations[k] = readToAnnotationStructure();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeInvisibleAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeInvisibleAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numAnnotations = reader.readget2();
            attr->annotations = new Annotation[attr->numAnnotations];
            FOR_EACH(k, attr->numAnnotations) {
                attr->annotations[k] = readToAnnotationStructure();
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeVisibleParameterAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeVisibleParameterAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numParameters = reader.readget1();
            attr->parameterAnnotations =
                new ATTR_RuntimeVisibleParameterAnnotations::
                    _parameterAnnotations[attr->numParameters];
            FOR_EACH(k, attr->numParameters) {
                attr->parameterAnnotations[k].numAnnotations =
                    reader.readget2();
                attr->parameterAnnotations[k].annotations =
                    new Annotation[attr->parameterAnnotations[k]
                                       .numAnnotations];
                FOR_EACH(p, attr->parameterAnnotations[k].numAnnotations) {
                    attr->parameterAnnotations[k].annotations[p] =
                        readToAnnotationStructure();
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeInvisibleParameterAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeInvisibleParameterAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numParameters = reader.readget1();
            attr->parameterAnnotations =
                new ATTR_RuntimeInvisibleParameterAnnotations::
                    _parameterAnnotations[attr->numParameters];
            FOR_EACH(k, attr->numParameters) {
                attr->parameterAnnotations[k].numAnnotations =
                    reader.readget2();
                attr->parameterAnnotations[k].annotations =
                    new Annotation[attr->parameterAnnotations[k]
                                       .numAnnotations];
                FOR_EACH(p, attr->parameterAnnotations[k].numAnnotations) {
                    attr->parameterAnnotations[k].annotations[p] =
                        readToAnnotationStructure();
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeVisibleTypeAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeVisibleTypeAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numAnnotations = reader.readget2();
            attr->annotations = new TypeAnnotation[attr->numAnnotations];
            FOR_EACH(k, attr->numAnnotations) {
                attr->annotations[k].targetType = reader.readget1();
                attr->annotations[k].targetInfo =
                    determineTargetType(attr->annotations[k].targetType);

                // read to target_path
                attr->annotations[k].targetPath.pathLength = reader.readget1();
                attr->annotations[k].targetPath.path = new TypeAnnotation::
                    TypePath::_path[attr->annotations[k].targetPath.pathLength];
                FOR_EACH(p, attr->annotations[k].targetPath.pathLength) {
                    attr->annotations[k].targetPath.path[p].typePathKind =
                        reader.readget1();
                    attr->annotations[k].targetPath.path[p].typeArgumentIndex =
                        reader.readget1();
                }

                attr->annotations[k].typeIndex = reader.readget2();
                attr->annotations[k].numElementValuePairs = reader.readget2();
                attr->annotations[k].elementValuePairs =
                    new TypeAnnotation::_elementValuePairs
                        [attr->annotations[k].numElementValuePairs];
                FOR_EACH(p, attr->annotations[k].numElementValuePairs) {
                    attr->annotations[k].elementValuePairs[p].elementNameIndex =
                        reader.readget2();
                    attr->annotations[k].elementValuePairs[p].value =
                        readToElementValueStructure();
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_RuntimeInvisibleTypeAnnotations(attrName) {
            auto* attr = new ATTR_RuntimeInvisibleTypeAnnotations;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numAnnotations = reader.readget2();
            attr->annotations = new TypeAnnotation[attr->numAnnotations];
            FOR_EACH(k, attr->numAnnotations) {
                attr->annotations[k].targetType = reader.readget1();
                attr->annotations[k].targetInfo =
                    determineTargetType(attr->annotations[k].targetType);

                // read to target_path
                attr->annotations[k].targetPath.pathLength = reader.readget1();
                attr->annotations[k].targetPath.path = new TypeAnnotation::
                    TypePath::_path[attr->annotations[k].targetPath.pathLength];
                FOR_EACH(p, attr->annotations[k].targetPath.pathLength) {
                    attr->annotations[k].targetPath.path[p].typePathKind =
                        reader.readget1();
                    attr->annotations[k].targetPath.path[p].typeArgumentIndex =
                        reader.readget1();
                }

                attr->annotations[k].typeIndex = reader.readget2();
                attr->annotations[k].numElementValuePairs = reader.readget2();
                attr->annotations[k].elementValuePairs =
                    new TypeAnnotation::_elementValuePairs
                        [attr->annotations[k].numElementValuePairs];
                FOR_EACH(p, attr->annotations[k].numElementValuePairs) {
                    attr->annotations[k].elementValuePairs[p].elementNameIndex =
                        reader.readget2();
                    attr->annotations[k].elementValuePairs[p].value =
                        readToElementValueStructure();
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_AnnotationDefault(attrName) {
            auto* attr = new ATTR_AnnotationDefault;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->defaultValue = readToElementValueStructure();
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_BootstrapMethods(attrName) {
            auto* attr = new ATTR_BootstrapMethods;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->numBootstrapMethods = reader.readget2();
            attr->bootstrapMethod = new ATTR_BootstrapMethods::_bootstrapMethod
                [attr->numBootstrapMethods];
            FOR_EACH(k, attr->numBootstrapMethods) {
                attr->bootstrapMethod[k].bootstrapMethodRef = reader.readget2();

                attr->bootstrapMethod[k].numBootstrapArgument =
                    reader.readget2();
                attr->bootstrapMethod[k].bootstrapArguments =
                    new u2[attr->bootstrapMethod[k].numBootstrapArgument];
                FOR_EACH(p, attr->bootstrapMethod[k].numBootstrapArgument) {
                    attr->bootstrapMethod[k].bootstrapArguments[p] =
                        reader.readget2();
                }
            }
            attrs[i] = attr;
            continue;
        }
        IS_ATTR_MethodParameters(attrName) {
            auto* attr = new ATTR_MethodParameter;
            attr->attributeNameIndex = attrStrIndex;
            attr->attributeLength = reader.readget4();
            attr->parameterCount = reader.readget1();
            attr->parameters =
                new ATTR_MethodParameter::_parameters[attr->parameterCount];
            FOR_EACH(k, attr->parameterCount) {
                attr->parameters[k].nameIndex = reader.readget2();
                attr->parameters[k].accessFlags = reader.readget2();
            }
            attrs[i] = attr;
        }
    }

    return true;
}

VerificationTypeInfo* JavaClass::determineVerificationType(u1 tag) {
    switch (tag) {
        case ITEM_Top: {
            return new VariableInfo_Top();
        }
        case ITEM_Integer: {
            return new VariableInfo_Integer;
        }
        case ITEM_Float: {
            return new VariableInfo_Float;
        }
        case ITEM_Null: {
            return new VariableInfo_Null;
        }
        case ITEM_UninitializedThis: {
            return new VariableInfo_UninitializedThis;
        }
        case ITEM_Object: {
            auto* x = new VariableInfo_Object;
            x->cpoolIndex = reader.readget2();
            return x;
        }
        case ITEM_Uninitialized: {
            auto* x = new VariableInfo_Uninitialized;
            x->offset = reader.readget2();
            return x;
        }
        case ITEM_Long: {
            return new VariableInfo_Long;
        }
        case ITEM_Double: {
            return new VariableInfo_Double;
        }
        default:
            cerr << __func__ << ":Incorrect tag of verification type\n";
            return nullptr;
    }
    SHOULD_NOT_REACH_HERE
}

TargetInfo* JavaClass::determineTargetType(u1 tag) {
    if (tag == 0x00 || tag == 0x01) {
        auto* t = new Target_TypeParameter;
        t->typeParameterIndex = reader.readget1();
        return t;
    }
    if (tag == 0x10) {
        auto* t = new Target_SuperType;
        t->superTypeIndex = reader.readget2();
        return t;
    }
    if (tag == 0x11 || tag == 0x12) {
        auto* t = new Target_TypeParameterBound;
        t->typeParameterIndex = reader.readget1();
        t->boundIndex = reader.readget1();
        return t;
    }
    if (tag == 0x13 || tag == 0x14 || tag == 0x15) {
        return new Target_Empty;
    }
    if (tag == 0x16) {
        auto* t = new Target_FormalParameter;
        t->formalParameter = reader.readget1();
        return t;
    }
    if (tag == 0x17) {
        auto* t = new Target_Throws;
        t->throwsTypeIndex = reader.readget2();
        return t;
    }
    if (tag == 0x40 || tag == 0x41) {
        auto* t = new Target_LocalVar;
        t->tableLength = reader.readget2();
        FOR_EACH(i, t->tableLength) {
            t->table[i].startPc = reader.readget2();
            t->table[i].length = reader.readget2();
            t->table[i].index = reader.readget2();
        }
        return t;
    }
    if (tag == 0x42) {
        auto* t = new Target_Catch;
        t->exceptionTableIndex = reader.readget2();
        return t;
    }
    if (tag >= 0x43 && tag <= 0x46) {
        auto* t = new Target_Offset;
        t->offset = reader.readget2();
        return t;
    }
    if (tag >= 0x47 && tag <= 0x4B) {
        auto* t = new Target_TypeArgument;
        t->offset = reader.readget2();
        t->typeArgumentIndex = reader.readget1();
        return t;
    }
    SHOULD_NOT_REACH_HERE
}

Annotation JavaClass::readToAnnotationStructure() {
    Annotation a{};
    a.typeIndex = reader.readget2();
    a.numElementValuePairs = reader.readget2();
    FOR_EACH(p, a.numElementValuePairs) {
        a.elementValuePairs[p].elementNameIndex = reader.readget2();
        a.elementValuePairs[p].value = readToElementValueStructure();
    }
    return a;
}

ElementValue* JavaClass::readToElementValueStructure() {
    char tag = reader.readget1();
    switch (tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'J':
        case 'S':
        case 'Z':
        case 's': {
            // const_value_index of union
            auto* e = new ElementValue_ConstantValueIndex;
            e->constValueIndex = reader.readget2();
            return e;
        }
        case 'e': {
            auto* e = new ElementValue_EnumConstValue;
            e->typeNameIndex = reader.readget2();
            e->constNameIndex = reader.readget2();
            break;
        }
        case 'c': {
            auto* e = new ElementValue_ClassInfoIndex;
            e->classInfoIndex = reader.readget2();
            return e;
        }
        case '@': {
            auto* e = new ElementValue_Annotation;
            e->annotationValue = readToAnnotationStructure();
            return e;
        }
        case '[': {
            auto* e = new ElementValue_ArrayValue;
            e->numValues = reader.readget2();
            e->values = new ElementValue*[e->numValues];
            FOR_EACH(i, e->numValues) {
                e->values[i] = readToElementValueStructure();
            }
            return e;
        }
        default:
            cerr << __func__ << ":Incorrect element value type\n";
            return nullptr;
    }

    SHOULD_NOT_REACH_HERE
}
