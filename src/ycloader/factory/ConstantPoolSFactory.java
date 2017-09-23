package ycloader.factory;

import ycloader.ClassFileReader;
import ycloader.adt.constantpool.*;
import ycloader.constant.ConstantPoolTags;
import ycloader.exception.ClassLoadingException;

public class ConstantPoolSFactory {
    public static AbstractConstantPool create(ClassFileReader reader, int tag) throws ClassLoadingException {
        switch (tag) {
            case ConstantPoolTags.CONSTANT_InterfaceMethodref:
                return new ConstantInterfaceMethodRefInfo(reader);
            case ConstantPoolTags.CONSTANT_Class:
                return new ConstantClassInfo(reader);
            case ConstantPoolTags.CONSTANT_Double:
                return new ConstantDoubleInfo(reader);
            case ConstantPoolTags.CONSTANT_Fieldref:
                return new ConstantFieldRefInfo(reader);
            case ConstantPoolTags.CONSTANT_Float:
                return new ConstantFloatInfo(reader);
            case ConstantPoolTags.CONSTANT_Integer:
                return new ConstantIntegerInfo(reader);
            case ConstantPoolTags.CONSTANT_Long:
                return new ConstantLongInfo(reader);
            case ConstantPoolTags.CONSTANT_MethodHandle:
                return new ConstantMethodHandleInfo(reader);
            case ConstantPoolTags.CONSTANT_Methodref:
                return new ConstantMethodRefInfo(reader);
            case ConstantPoolTags.CONSTANT_MethodType:
                return new ConstantMethodTypeInfo(reader);
            case ConstantPoolTags.CONSTANT_NameAndType:
                return new ConstantNameAndTypeInfo(reader);
            case ConstantPoolTags.CONSTANT_String:
                return new ConstantStringInfo(reader);
            case ConstantPoolTags.CONSTANT_Utf8:
                return new ConstantUtf8Info(reader);
            case ConstantPoolTags.CONSTANT_InvokeDynamic:
                return new ConstantInvokeDynamicInfo(reader);
            default:
                throw new ClassLoadingException("unknow constant pool entry name");
        }
    }
}
