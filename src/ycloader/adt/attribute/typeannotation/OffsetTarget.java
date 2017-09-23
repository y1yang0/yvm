package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class OffsetTarget extends Target {
    private u2 offset;

    public OffsetTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        offset = read2Bytes();
    }

    public int getActualBytes() {
        return 2;
    }
}
