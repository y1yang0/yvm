package ycloader.dataobject;

import ycloader.ClassFileReader;

/**
 * exactly same internal structure with FieldObject and its actions
 */
public class MethodObject extends FieldObject {
    public MethodObject(ClassFileReader reader, ConstantPoolObject cpObject, int fieldCount) {
        super(reader, cpObject, fieldCount);
    }
}
