package ycloader.adt.constantpool;

import ycloader.ClassFileReader;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.constant.ConstantPoolTags;

import java.io.IOException;

public class ConstantInvokeDynamicInfo extends AbstractConstantPool {
    public static final u1 tag = new u1(ConstantPoolTags.CONSTANT_InterfaceMethodref);
    public u2 bootstrapMethodAttrIndex;
    public u2 nameAndTypeIndex;

    public ConstantInvokeDynamicInfo(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        bootstrapMethodAttrIndex = read2Bytes();
        nameAndTypeIndex = read2Bytes();
    }
}
