package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class ExceptionAttribute extends Attribute {
    private u2 numberOfExceptions;
    private u2[] exceptionIndexTable;

    public ExceptionAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        numberOfExceptions = read2Bytes();
        exceptionIndexTable = new u2[numberOfExceptions.getValue()];
        for (int i = 0; i < numberOfExceptions.getValue(); i++) {
            exceptionIndexTable[i] = read2Bytes();
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        return 2 + exceptionIndexTable.length * 2;
    }
}
