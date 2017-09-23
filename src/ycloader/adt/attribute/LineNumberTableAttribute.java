package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class LineNumberTableAttribute extends Attribute {
    private u2 lineNumberTableLength;
    private LineNumber[] lineNumberTable;

    public LineNumberTableAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        lineNumberTableLength = read2Bytes();
        lineNumberTable = new LineNumber[lineNumberTableLength.getValue()];
        for (int i = 0; i < lineNumberTableLength.getValue(); i++) {
            LineNumber x = new LineNumber();
            x.stuffing();
            lineNumberTable[i] = x;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (LineNumber ln : lineNumberTable) {
            res += ln.getActualBytes();
        }
        return res;
    }

    class LineNumber {
        u2 startPc;
        u2 lineNumber;

        void stuffing() throws IOException {
            startPc = read2Bytes();
            lineNumber = read2Bytes();
        }

        int getActualBytes() {
            return 4;
        }
    }
}
