package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class LocalVariableTableAttribute extends Attribute {
    private u2 localVariableTableLength;
    private LocalVariable[] localVariableTable;

    public LocalVariableTableAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        localVariableTableLength = read2Bytes();
        localVariableTable = new LocalVariable[localVariableTableLength.getValue()];
        for (int i = 0; i < localVariableTableLength.getValue(); i++) {
            LocalVariable var = new LocalVariable();
            var.stuffing();
            localVariableTable[i] = var;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (LocalVariable lv : localVariableTable) {
            res += lv.getActualBytes();
        }
        return res;
    }

    class LocalVariable {
        u2 startPc;
        u2 length;
        u2 nameIndex;
        u2 descriptorIndex;
        u2 index;

        void stuffing() throws IOException {
            startPc = read2Bytes();
            length = read2Bytes();
            nameIndex = read2Bytes();
            descriptorIndex = read2Bytes();
            index = read2Bytes();
        }

        int getActualBytes() {
            return 10;
        }
    }
}
