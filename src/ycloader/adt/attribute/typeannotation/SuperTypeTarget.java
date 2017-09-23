package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;


public class SuperTypeTarget extends Target {
    private u2 superTypeIndex;

    public SuperTypeTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        superTypeIndex = read2Bytes();
    }

    public int getActualBytes() {
        return 2;
    }
}
