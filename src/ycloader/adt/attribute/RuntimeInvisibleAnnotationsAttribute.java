package ycloader.adt.attribute;

import ycloader.ClassFileReader;

public class RuntimeInvisibleAnnotationsAttribute extends RuntimeVisibleAnnotationsAttribute {
    public RuntimeInvisibleAnnotationsAttribute(ClassFileReader reader) {
        super(reader);
    }
}
