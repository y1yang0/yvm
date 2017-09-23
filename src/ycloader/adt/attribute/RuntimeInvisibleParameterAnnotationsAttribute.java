package ycloader.adt.attribute;

import ycloader.ClassFileReader;

public class RuntimeInvisibleParameterAnnotationsAttribute extends RuntimeVisibleParameterAnnotationsAttribute {
    public RuntimeInvisibleParameterAnnotationsAttribute(ClassFileReader reader) {
        super(reader);
    }
}
