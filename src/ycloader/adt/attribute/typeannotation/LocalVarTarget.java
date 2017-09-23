package ycloader.adt.attribute.typeannotation;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;


public class LocalVarTarget extends Target {
    private u2 tableLength;
    private LocalVarTarget.CodeEntry[] table;

    public LocalVarTarget(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        tableLength = read2Bytes();
        table = new LocalVarTarget.CodeEntry[tableLength.getValue()];
        for (int i = 0; i < tableLength.getValue(); i++) {
            LocalVarTarget.CodeEntry ce = new LocalVarTarget.CodeEntry();
            ce.stuffing();
            table[i] = ce;
        }
    }

    @Override
    public int getActualBytes() {
        int res = 2;
        for (CodeEntry ce : table) {
            res += ce.getActualBytes();
        }
        return res;
    }

    class CodeEntry {
        u2 startPc;
        u2 length;
        u2 index;

        void stuffing() throws IOException {
            startPc = read2Bytes();
            length = read2Bytes();
            index = read2Bytes();
        }

        int getActualBytes() {
            return 6;
        }
    }
}
