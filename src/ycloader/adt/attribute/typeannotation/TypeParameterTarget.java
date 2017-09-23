package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u1;

import java.io.IOException;


public class TypeParameterTarget extends Target {
    private u1 typeParameterIndex;

    public TypeParameterTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        typeParameterIndex = read1Byte();
    }

    public int getActualBytes() {
        return 1;
    }
}
