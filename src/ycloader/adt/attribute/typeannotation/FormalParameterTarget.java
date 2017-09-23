package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u1;

import java.io.IOException;


public class FormalParameterTarget extends Target {
    private u1 formalParameterIndex;

    public FormalParameterTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        formalParameterIndex = read1Byte();
    }

    @Override
    public int getActualBytes() {
        return 1;
    }
}
