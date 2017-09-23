package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;

import java.io.IOException;

public class BootstrapMethodsAttribute extends Attribute {
    public u2 numBootstrapMethods;
    private BootstrapMethod[] bootstrapMethods;

    public BootstrapMethodsAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        numBootstrapMethods = read2Bytes();
        bootstrapMethods = new BootstrapMethod[numBootstrapMethods.getValue()];
        for (int i = 0; i < numBootstrapMethods.getValue(); i++) {
            BootstrapMethod bm = new BootstrapMethod();
            bm.stuffing();
            bootstrapMethods[i] = bm;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (BootstrapMethod bm : bootstrapMethods) {
            res += bm.getActualBytes();
        }
        return res;
    }

    class BootstrapMethod {
        u2 bootstrapMethodRef;
        u2 numBootstrapArgument;
        u2[] bootstrapArguments;

        void stuffing() throws IOException {
            bootstrapMethodRef = read2Bytes();
            numBootstrapArgument = read2Bytes();
            bootstrapArguments = new u2[numBootstrapArgument.getValue()];
            for (int i = 0; i < numBootstrapArgument.getValue(); i++) {
                bootstrapArguments[i] = read2Bytes();
            }
        }

        int getActualBytes() {
            return 4 + bootstrapArguments.length * 2;
        }
    }

}
