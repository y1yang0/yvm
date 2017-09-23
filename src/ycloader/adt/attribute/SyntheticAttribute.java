package ycloader.adt.attribute;

import ycloader.ClassFileReader;

import java.io.IOException;

public class SyntheticAttribute extends Attribute {
    public SyntheticAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        return 0;
    }
}
