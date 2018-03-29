#include "Debug.h"
#include "AccessFlag.h"
#include "JavaType.h"

void Inspector::printConstantPool(const JavaClass& jc) {
    using namespace std;
    DbgPleasant d("Constant pool", 3);
    d.setCellWidth(30);
    d.addCell("Index");
    d.addCell("Slot Type");
    d.addCell("Extra information");
    for (int i = 1; i <= jc.raw.constPoolCount - 1; i++) {
        d.addCell("#"+std::to_string(i));
        d.addCell(typeid(*jc.raw.constPoolInfo[i]).name());

        // Extra information about specified CONSTANT_* structure
        if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Utf8)) {
            d.addCell((char*)dynamic_cast<CONSTANT_Utf8*>(jc.raw.constPoolInfo[i])->bytes);
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_String)) {
            d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_String*>(jc.raw.constPoolInfo[i])->stringIndex));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Integer)) {
            d.addCell(std::to_string(dynamic_cast<CONSTANT_Integer*>(jc.raw.constPoolInfo[i])->val));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Float)) {
            d.addCell(std::to_string(dynamic_cast<CONSTANT_Float*>(jc.raw.constPoolInfo[i])->val));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Long)) {
            d.addCell(std::to_string(dynamic_cast<CONSTANT_Long*>(jc.raw.constPoolInfo[i])->val));
            i++;
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Double)) {
            d.addCell(std::to_string(dynamic_cast<CONSTANT_Double*>(jc.raw.constPoolInfo[i])->val));
            i++;
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Fieldref)) {
            d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_NameAndType*>(jc.raw.constPoolInfo[dynamic_cast<CONSTANT_Fieldref*>(jc.raw.constPoolInfo[i])->nameAndTypeIndex])->nameIndex));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Methodref)) {
            d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_NameAndType*>(jc.raw.constPoolInfo[dynamic_cast<CONSTANT_Methodref*>(jc.raw.constPoolInfo[i])->nameAndTypeIndex])->nameIndex));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_InterfaceMethodref)) {
            d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_NameAndType*>(jc.raw.constPoolInfo[dynamic_cast<CONSTANT_InterfaceMethodref*>(jc.raw.constPoolInfo[i])->nameAndTypeIndex])->nameIndex));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_Class)) {
            d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_Class*>(jc.raw.constPoolInfo[i])->nameIndex));
        }
        else if (typeid(*jc.raw.constPoolInfo[i]) == typeid(CONSTANT_NameAndType)) {
            std::string nameAndType;
            nameAndType += (char*)jc.getString(dynamic_cast<CONSTANT_NameAndType*>(jc.raw.constPoolInfo[i])->nameIndex);
            nameAndType += " ! ";
            nameAndType += (char*)jc.getString(dynamic_cast<CONSTANT_NameAndType*>(jc.raw.constPoolInfo[i])->descriptorIndex);
            d.addCell(nameAndType);
        }
        else {
            d.addCell(" ");
        }
    }
    d.show();
}

void Inspector::printField(const JavaClass &jc) {
    using namespace std;
    DbgPleasant d("Class fields",4);
    d.setCellWidth(30);
    d.addCell("Flags");
    d.addCell("Field name");
    d.addCell("Field descriptor");
    d.addCell("Attrs count");
    FOR_EACH(i,jc.raw.fieldsCount){
        int flag = jc.raw.fields[i].accessFlags;

        std::string flagstr;
        if (IS_FIELD_PUBLIC(flag)) {
            flagstr += "[public]";
        }
        else if (IS_FIELD_PRIVATE(flag)) {
            flagstr += "[private]";
        }
        else if (IS_FIELD_PROTECTED(flag)) {
            flagstr += "[protected]";
        }

        if (IS_FIELD_STATIC(flag)) {
            flagstr += "[static]";
        }
        if (IS_FIELD_FINAL(flag)) {
            flagstr += "[final]";
        }
        if (IS_FIELD_VOLATILE(flag)) {
            flagstr += "[volatile]";
        }
        if (IS_FIELD_TRANSIENT(flag)) {
            flagstr += "[transient]";
        }
        if (IS_FIELD_SYNTHETIC(flag)) {
            flagstr += "[synthetic_by_compiler]";
        }
        if (IS_FIELD_ENUM(flag)) {
            flagstr += "[enum]";
        }
        d.addCell(flagstr);
        d.addCell((char*)jc.getString(jc.raw.fields[i].nameIndex));
        d.addCell((char*)jc.getString(jc.raw.fields[i].descriptorIndex));
        d.addCell(std::to_string(jc.raw.fields[i].attributeCount));
    }
    d.show();
}

void Inspector::printClassAccessFlag(const JavaClass & jc) {
    using namespace std;
    DbgPleasant d("Class access flags",1);
    d.addCell("Flags");

    std::string flagstr;
    if (IS_CLASS_PUBLIC(jc.raw.accessFlags)) {
        flagstr += "[public] ";
    }
    if (IS_CLASS_ABSTRACT(jc.raw.accessFlags)) {
        flagstr += "[abstract] ";
    }
    if (IS_CLASS_FINAL(jc.raw.accessFlags)) {
        flagstr += "[final] ";
    }
    if (IS_CLASS_SUPER(jc.raw.accessFlags)) {
        flagstr += "[super] ";
    }
    if (IS_CLASS_INTERFACE(jc.raw.accessFlags)) {
        flagstr += "[interface] ";
    }
    if (IS_CLASS_SYNTHETIC(jc.raw.accessFlags)) {
        flagstr += "[synthetic_by_compiler] ";
    }
    if (IS_CLASS_ANNOTATION(jc.raw.accessFlags)) {
        flagstr += "[annotation] ";
    }
    if (IS_CLASS_ENUM(jc.raw.accessFlags)) {
        flagstr += "[enum] ";
    }
    d.addCell(flagstr);
    d.show();
}

void Inspector::printMethod(const JavaClass &jc) {
    using namespace std;
    DbgPleasant d("Methods", 5);
    d.addCell("Flags");
    d.addCell("Method name");
    d.addCell("Method descriptor");
    d.addCell("Attrs count");
    d.addCell("Attrs type");
    FOR_EACH(i,jc.raw.methodsCount){
        std::string flagstr;

        if (IS_METHOD_PUBLIC(jc.raw.methods[i].accessFlags)) {
            flagstr += "[public]";
        }
        else if (IS_METHOD_PRIVATE(jc.raw.methods[i].accessFlags)) {
            flagstr += "[private]";
        }
        else if (IS_METHOD_PROTECTED(jc.raw.methods[i].accessFlags)) {
            flagstr += "[protected]";
        }

        if (IS_METHOD_STATIC(jc.raw.methods[i].accessFlags)) {
            flagstr += "[static]";
        }
        if (IS_METHOD_FINAL(jc.raw.methods[i].accessFlags)) {
            flagstr += "[final]";
        }
        if (IS_METHOD_SYNCHRONIZED(jc.raw.methods[i].accessFlags)) {
            flagstr += "[synchronized]";
        }
        if (IS_METHOD_BRIDGE(jc.raw.methods[i].accessFlags)) {
            flagstr += "[bridge]";
        }
        if (IS_METHOD_VARARGS(jc.raw.methods[i].accessFlags)) {
            cout <<" [VARARGS]";
        }
        if (IS_METHOD_NATIVE(jc.raw.methods[i].accessFlags)) {
            flagstr += "[native]";
        }
        if (IS_METHOD_ABSTRACT(jc.raw.methods[i].accessFlags)) {
            flagstr += "[abstract]";
        }
        if (IS_METHOD_STRICT(jc.raw.methods[i].accessFlags)) {
            flagstr += "[strictfp_mode]";
        }
        if (IS_METHOD_SYNTHETIC(jc.raw.methods[i].accessFlags)) {
            flagstr += "[synthetic]";
        }

        d.addCell(flagstr);
        d.addCell((char*)jc.getString(jc.raw.methods[i].nameIndex));
        d.addCell((char*)jc.getString(jc.raw.methods[i].descriptorIndex));
        d.addCell(std::to_string(jc.raw.methods[i].attributeCount));
        std::string attrsType;
        FOR_EACH(k, jc.raw.methods[i].attributeCount) {
            attrsType.append(typeid(*jc.raw.methods[i].attributes[k]).name());
            attrsType.append(" ");
        }
        d.addCell(attrsType);
    }
    d.show();
}

void Inspector::printClassFileVersion(const JavaClass & jc){
    DbgPleasant d("Class file versions", 2);
    d.addCell("Minor version");
    d.addCell(std::to_string(jc.raw.minorVersion));
    d.addCell("Major version");
    d.addCell(std::to_string(jc.raw.majorVersion));
    d.show();
}

void Inspector::printInterfaces(const JavaClass & jc){
    DbgPleasant d("Interfaces", 2);
    d.addCell("Index");
    d.addCell("Interface name");
    FOR_EACH(i, jc.raw.interfacesCount) {
        d.addCell("#" + std::to_string(i));
        d.addCell((char*)jc.getString(dynamic_cast<CONSTANT_Class*>(jc.raw.constPoolInfo[jc.raw.interfaces[i]])->nameIndex));
    }
    d.show();
}

void Inspector::printClassFileAttrs(const JavaClass & jc){
    DbgPleasant d("Class file attributes", 2);
    d.addCell("Attribute type");
    d.addCell("Attribute byte length");
    FOR_EACH(i,jc.raw.attributesCount) {
        d.addCell(typeid(*jc.raw.attributes[i]).name());
        d.addCell(std::to_string(jc.raw.attributes[i]->attributeLength));
    }
    d.show();
}

void Inspector::printSizeofInternalTypes(){
    DbgPleasant d("All size of internal types", 2);
    d.addCell("Type");
    d.addCell("Bytes");

    d.addCell(typeid(JDouble).name());
    d.addCell(std::to_string(sizeof(JDouble)));
    d.addCell(typeid(JFloat).name());
    d.addCell(std::to_string(sizeof(JFloat)));
    d.addCell(typeid(JInt).name());
    d.addCell(std::to_string(sizeof(JInt)));
    d.addCell(typeid(JLong).name());
    d.addCell(std::to_string(sizeof(JLong)));
    d.addCell(typeid(JObject).name());
    d.addCell(std::to_string(sizeof(JObject)));
    d.addCell(typeid(JArray).name());
    d.addCell(std::to_string(sizeof(JArray)));
    d.show();
}

void Inspector::printOpcode(u1 * code, u4 index){
    switch (code[index]) {
    case 0: std::cout << "nop\n"; break;
    case 1: std::cout << "aconst_null\n"; break;
    case 2: std::cout << "iconst_m1\n"; break;
    case 3: std::cout << "iconst_0\n"; break;
    case 4: std::cout << "iconst_1\n"; break;
    case 5: std::cout << "iconst_2\n"; break;
    case 6: std::cout << "iconst_3\n"; break;
    case 7: std::cout << "iconst_4\n"; break;
    case 8: std::cout << "iconst_5\n"; break;
    case 9: std::cout << "lconst_0\n"; break;
    case 10: std::cout << "lconst_1\n"; break;
    case 11: std::cout << "fconst_0\n"; break;
    case 12: std::cout << "fconst_1\n"; break;
    case 13: std::cout << "fconst_2\n"; break;
    case 14: std::cout << "dconst_0\n"; break;
    case 15: std::cout << "dconst_1\n"; break;
    case 16: std::cout << "bipush\n"; break;
    case 17: std::cout << "sipush\n"; break;
    case 18: std::cout << "ldc\n"; break;
    case 19: std::cout << "ldc_w\n"; break;
    case 20: std::cout << "ldc2_w\n"; break;
    case 21: std::cout << "iload\n"; break;
    case 22: std::cout << "lload\n"; break;
    case 23: std::cout << "fload\n"; break;
    case 24: std::cout << "dload\n"; break;
    case 25: std::cout << "aload\n"; break;
    case 26: std::cout << "iload_0\n"; break;
    case 27: std::cout << "iload_1\n"; break;
    case 28: std::cout << "iload_2\n"; break;
    case 29: std::cout << "iload_3\n"; break;
    case 30: std::cout << "lload_0\n"; break;
    case 31: std::cout << "lload_1\n"; break;
    case 32: std::cout << "lload_2\n"; break;
    case 33: std::cout << "lload_3\n"; break;
    case 34: std::cout << "fload_0\n"; break;
    case 35: std::cout << "fload_1\n"; break;
    case 36: std::cout << "fload_2\n"; break;
    case 37: std::cout << "fload_3\n"; break;
    case 38: std::cout << "dload_0\n"; break;
    case 39: std::cout << "dload_1\n"; break;
    case 40: std::cout << "dload_2\n"; break;
    case 41: std::cout << "dload_3\n"; break;
    case 42: std::cout << "aload_0\n"; break;
    case 43: std::cout << "aload_1\n"; break;
    case 44: std::cout << "aload_2\n"; break;
    case 45: std::cout << "aload_3\n"; break;
    case 46: std::cout << "iaload\n"; break;
    case 47: std::cout << "laload\n"; break;
    case 48: std::cout << "faload\n"; break;
    case 49: std::cout << "daload\n"; break;
    case 50: std::cout << "aaload\n"; break;
    case 51: std::cout << "baload\n"; break;
    case 52: std::cout << "caload\n"; break;
    case 53: std::cout << "saload\n"; break;
    case 54: std::cout << "istore\n"; break;
    case 55: std::cout << "lstore\n"; break;
    case 56: std::cout << "fstore\n"; break;
    case 57: std::cout << "dstore\n"; break;
    case 58: std::cout << "astore\n"; break;
    case 59: std::cout << "istore_0\n"; break;
    case 60: std::cout << "istore_1\n"; break;
    case 61: std::cout << "istore_2\n"; break;
    case 62: std::cout << "istore_3\n"; break;
    case 63: std::cout << "lstore_0\n"; break;
    case 64: std::cout << "lstore_1\n"; break;
    case 65: std::cout << "lstore_2\n"; break;
    case 66: std::cout << "lstore_3\n"; break;
    case 67: std::cout << "fstore_0\n"; break;
    case 68: std::cout << "fstore_1\n"; break;
    case 69: std::cout << "fstore_2\n"; break;
    case 70: std::cout << "fstore_3\n"; break;
    case 71: std::cout << "dstore_0\n"; break;
    case 72: std::cout << "dstore_1\n"; break;
    case 73: std::cout << "dstore_2\n"; break;
    case 74: std::cout << "dstore_3\n"; break;
    case 75: std::cout << "astore_0\n"; break;
    case 76: std::cout << "astore_1\n"; break;
    case 77: std::cout << "astore_2\n"; break;
    case 78: std::cout << "astore_3\n"; break;
    case 79: std::cout << "iastore\n"; break;
    case 80: std::cout << "lastore\n"; break;
    case 81: std::cout << "fastore\n"; break;
    case 82: std::cout << "dastore\n"; break;
    case 83: std::cout << "aastore\n"; break;
    case 84: std::cout << "bastore\n"; break;
    case 85: std::cout << "castore\n"; break;
    case 86: std::cout << "sastore\n"; break;
    case 87: std::cout << "pop\n"; break;
    case 88: std::cout << "pop2\n"; break;
    case 89: std::cout << "dup\n"; break;
    case 90: std::cout << "dup_x1\n"; break;
    case 91: std::cout << "dup_x2\n"; break;
    case 92: std::cout << "dup2\n"; break;
    case 93: std::cout << "dup2_x1\n"; break;
    case 94: std::cout << "dup2_x2\n"; break;
    case 95: std::cout << "swap\n"; break;
    case 96: std::cout << "iadd\n"; break;
    case 97: std::cout << "ladd\n"; break;
    case 98: std::cout << "fadd\n"; break;
    case 99: std::cout << "dadd\n"; break;
    case 100: std::cout << "isub\n"; break;
    case 101: std::cout << "lsub\n"; break;
    case 102: std::cout << "fsub\n"; break;
    case 103: std::cout << "dsub\n"; break;
    case 104: std::cout << "imul\n"; break;
    case 105: std::cout << "lmul\n"; break;
    case 106: std::cout << "fmul\n"; break;
    case 107: std::cout << "dmul\n"; break;
    case 108: std::cout << "idiv\n"; break;
    case 109: std::cout << "ldiv\n"; break;
    case 110: std::cout << "fdiv\n"; break;
    case 111: std::cout << "ddiv\n"; break;
    case 112: std::cout << "irem\n"; break;
    case 113: std::cout << "lrem\n"; break;
    case 114: std::cout << "frem\n"; break;
    case 115: std::cout << "drem\n"; break;
    case 116: std::cout << "ineg\n"; break;
    case 117: std::cout << "lneg\n"; break;
    case 118: std::cout << "fneg\n"; break;
    case 119: std::cout << "dneg\n"; break;
    case 120: std::cout << "ishl\n"; break;
    case 121: std::cout << "lshl\n"; break;
    case 122: std::cout << "ishr\n"; break;
    case 123: std::cout << "lshr\n"; break;
    case 124: std::cout << "iushr\n"; break;
    case 125: std::cout << "lushr\n"; break;
    case 126: std::cout << "iand\n"; break;
    case 127: std::cout << "land\n"; break;
    case 128: std::cout << "ior\n"; break;
    case 129: std::cout << "lor\n"; break;
    case 130: std::cout << "ixor\n"; break;
    case 131: std::cout << "lxor\n"; break;
    case 132: std::cout << "iinc\n"; break;
    case 133: std::cout << "i2l\n"; break;
    case 134: std::cout << "i2f\n"; break;
    case 135: std::cout << "i2d\n"; break;
    case 136: std::cout << "l2i\n"; break;
    case 137: std::cout << "l2f\n"; break;
    case 138: std::cout << "l2d\n"; break;
    case 139: std::cout << "f2i\n"; break;
    case 140: std::cout << "f2l\n"; break;
    case 141: std::cout << "f2d\n"; break;
    case 142: std::cout << "d2i\n"; break;
    case 143: std::cout << "d2l\n"; break;
    case 144: std::cout << "d2f\n"; break;
    case 145: std::cout << "i2b\n"; break;
    case 146: std::cout << "i2c\n"; break;
    case 147: std::cout << "i2s\n"; break;
    case 148: std::cout << "lcmp\n"; break;
    case 149: std::cout << "fcmpl\n"; break;
    case 150: std::cout << "fcmpg\n"; break;
    case 151: std::cout << "dcmpl\n"; break;
    case 152: std::cout << "dcmpg\n"; break;
    case 153: std::cout << "ifeq\n"; break;
    case 154: std::cout << "ifne\n"; break;
    case 155: std::cout << "iflt\n"; break;
    case 156: std::cout << "ifge\n"; break;
    case 157: std::cout << "ifgt\n"; break;
    case 158: std::cout << "ifle\n"; break;
    case 159: std::cout << "if_icmpeq\n"; break;
    case 160: std::cout << "if_icmpne\n"; break;
    case 161: std::cout << "if_icmplt\n"; break;
    case 162: std::cout << "if_icmpge\n"; break;
    case 163: std::cout << "if_icmpgt\n"; break;
    case 164: std::cout << "if_icmple\n"; break;
    case 165: std::cout << "if_acmpeq\n"; break;
    case 166: std::cout << "if_acmpne\n"; break;
    case 167: std::cout << "goto\n"; break;
    case 168: std::cout << "jsr\n"; break;
    case 169: std::cout << "ret\n"; break;
    case 170: std::cout << "tableswitch\n"; break;
    case 171: std::cout << "lookupswitch\n"; break;
    case 172: std::cout << "ireturn\n"; break;
    case 173: std::cout << "lreturn\n"; break;
    case 174: std::cout << "freturn\n"; break;
    case 175: std::cout << "dreturn\n"; break;
    case 176: std::cout << "areturn\n"; break;
    case 177: std::cout << "return\n"; break;
    case 178: std::cout << "getstatic\n"; break;
    case 179: std::cout << "putstatic\n"; break;
    case 180: std::cout << "getfield\n"; break;
    case 181: std::cout << "putfield\n"; break;
    case 182: std::cout << "invokevirtual\n"; break;
    case 183: std::cout << "invokespecial\n"; break;
    case 184: std::cout << "invokestatic\n"; break;
    case 185: std::cout << "invokeinterface\n"; break;
    case 186: std::cout << "invokedynamic\n"; break;
    case 187: std::cout << "new\n"; break;
    case 188: std::cout << "newarray\n"; break;
    case 189: std::cout << "anewarray\n"; break;
    case 190: std::cout << "arraylength\n"; break;
    case 191: std::cout << "athrow\n"; break;
    case 192: std::cout << "checkcast\n"; break;
    case 193: std::cout << "instanceof\n"; break;
    case 194: std::cout << "monitorenter\n"; break;
    case 195: std::cout << "monitorexit\n"; break;
    case 196: std::cout << "wide\n"; break;
    case 197: std::cout << "multianewarray\n"; break;
    case 198: std::cout << "ifnull\n"; break;
    case 199: std::cout << "ifnonnull\n"; break;
    case 200: std::cout << "goto_w\n"; break;
    case 201: std::cout << "jsr_w\n"; break;
    case 202: std::cout << "breakpoint\n"; break;
    case 254: std::cout << "impdep1\n"; break;
    case 255: std::cout << "impdep2\n"; break;
    default:
        std::cout << "Invalid opcode detected!\n";
    }
}
