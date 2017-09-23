package ycloader.adt.attribute;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.u2;
import ycloader.adt.u4;

public abstract class Attribute extends BytesReaderProxy implements Stuffable, Verifiable {
    public u2 attributeNameIndex;
    protected u4 attributeLength;

    protected Attribute(ClassFileReader reader) {
        super(reader);
    }

    public final boolean isValidAttribute() {
        return attributeLength.getValue() == getActualBytes();
    }
}