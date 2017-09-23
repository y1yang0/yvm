package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class LocalVariableTypeTableAttribute extends Attribute {
    private u2 localVariableTypeTableLength;
    private LocalVariableType[] localVariableTypeTable;

    public LocalVariableTypeTableAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        localVariableTypeTableLength = read2Bytes();
        localVariableTypeTable = new LocalVariableType[localVariableTypeTableLength.getValue()];
        for (int i = 0; i < localVariableTypeTableLength.getValue(); i++) {
            LocalVariableType varType = new LocalVariableType();
            varType.stuffing();
            localVariableTypeTable[i] = varType;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (LocalVariableType lvt : localVariableTypeTable) {
            res += lvt.getActualBytes();
        }
        return res;
    }

    class LocalVariableType {
        u2 startPc;
        u2 length;
        u2 nameIndex;
        u2 signatureIndex;
        u2 index;

        void stuffing() throws IOException {
            startPc = read2Bytes();
            length = read2Bytes();
            nameIndex = read2Bytes();
            signatureIndex = read2Bytes();
            index = read2Bytes();
        }

        int getActualBytes() {
            return 10;
        }
    }
}
