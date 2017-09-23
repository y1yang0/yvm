package ycloader.adt.constantpool;

import ycloader.ClassFileReader;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.constant.ConstantPoolTags;

import java.io.IOException;

public class ConstantNameAndTypeInfo extends AbstractConstantPool {
    public static final u1 tag = new u1(ConstantPoolTags.CONSTANT_NameAndType);
    public u2 nameIndex;
    public u2 descriptorIndex;

    public ConstantNameAndTypeInfo(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        nameIndex = read2Bytes();
        descriptorIndex = read2Bytes();
    }
}
