package ycloader.adt.constantpool;

import ycloader.ClassFileReader;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.constant.ConstantPoolTags;

import java.io.IOException;

public class ConstantMethodRefInfo extends AbstractConstantPool {
    public static final u1 tag = new u1(ConstantPoolTags.CONSTANT_Methodref);
    public u2 classIndex;
    public u2 nameAndTypeIndex;

    public ConstantMethodRefInfo(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        classIndex = read2Bytes();
        nameAndTypeIndex = read2Bytes();
    }
}
