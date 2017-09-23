package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class InnerClassAttribute extends Attribute {
    private u2 numberOfClasses;
    private CPIndexBundle[] classes;

    public InnerClassAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        numberOfClasses = read2Bytes();
        classes = new CPIndexBundle[numberOfClasses.getValue()];
        for (int i = 0; i < numberOfClasses.getValue(); i++) {
            CPIndexBundle bundle = new CPIndexBundle();
            bundle.stuffing();
            classes[i] = bundle;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (CPIndexBundle cpb : classes) {
            res += cpb.getActualBytes();
        }
        return res;
    }

    class CPIndexBundle {
        u2 innerClassInfoIndex;
        u2 outerClassInfoIndex;
        u2 innerNameIndex;
        u2 innerClassAccessFlags;

        void stuffing() throws IOException {
            innerClassInfoIndex = read2Bytes();
            outerClassInfoIndex = read2Bytes();
            innerNameIndex = read2Bytes();
            innerClassAccessFlags = read2Bytes();
        }

        int getActualBytes() {
            return 8;
        }
    }
}
