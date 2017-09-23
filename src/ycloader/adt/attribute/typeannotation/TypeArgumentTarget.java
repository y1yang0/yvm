package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u1;
import ycloader.adt.u2;

import java.io.IOException;

public class TypeArgumentTarget extends Target {
    private u2 offset;
    private u1 typeArgumentIndex;

    public TypeArgumentTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        offset = read2Bytes();
        typeArgumentIndex = read1Byte();
    }

    public int getActualBytes() {
        return 3;
    }
}
