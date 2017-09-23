package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u1;
import ycloader.adt.u2;

import java.io.IOException;

public class MethodParametersAttribute extends Attribute {
    private u1 parametersCount;
    private Parameter[] parameters;

    public MethodParametersAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException {
        attributeLength = read4Bytes();
        parametersCount = read1Byte();
        parameters = new Parameter[parametersCount.getValue()];
        for (int i = 0; i < parametersCount.getValue(); i++) {
            Parameter p = new Parameter();
            p.stuffing();
            parameters[i] = p;
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 1;
        for (Parameter p : parameters) {
            res += p.getActualBytes();
        }
        return res;
    }

    class Parameter {
        u2 nameIndex;
        u2 accessFlags;

        void stuffing() throws IOException {
            nameIndex = read2Bytes();
            accessFlags = read2Bytes();
        }

        int getActualBytes() {
            return 4;
        }
    }
}
