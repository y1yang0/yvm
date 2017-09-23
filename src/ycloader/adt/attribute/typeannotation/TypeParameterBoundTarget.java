package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u1;

import java.io.IOException;


public class TypeParameterBoundTarget extends Target {
    private u1 typeParameterIndex;
    private u1 boundIndex;

    public TypeParameterBoundTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        typeParameterIndex = read1Byte();
        boundIndex = read1Byte();
    }

    public int getActualBytes() {
        return 2;
    }
}
