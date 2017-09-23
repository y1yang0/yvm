package ycloader.dataobject;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.CodeAttribute;
import ycloader.adt.constantpool.AbstractConstantPool;
import ycloader.adt.constantpool.ConstantUtf8Info;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.adt.u4;
import ycloader.exception.ClassLoadingException;
import ycloader.factory.AttributeFactory;

import java.io.IOException;
import java.util.ArrayList;

public class ClassFileAttributeObject extends BytesReaderProxy implements Stuffable {
    private int attributeCount;
    private ArrayList<Attribute> attributes;
    private u1[] customeAttribute;
    private ConstantPoolObject constantPoolObject;

    public ClassFileAttributeObject(ClassFileReader reader, ConstantPoolObject cpObject, int attributeCount) {
        super(reader);
        this.constantPoolObject = cpObject;
        this.attributeCount = attributeCount;
        attributes = new ArrayList<>();
    }

    public ArrayList<Attribute> getClassFileAttributes() {
        return attributes;
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        for (int i = 0; i < attributeCount; i++) {
            u2 attributeNameIndex = read2Bytes();

            //We need to check if it's a \"ConstantUtf8Info\" instance"
            AbstractConstantPool cb = constantPoolObject.at(attributeNameIndex.getValue());
            //if it's not a ycloader.constant utf-8 info structure then throws an ycloader.exception
            if (cb instanceof ConstantUtf8Info) {
                Attribute attr = AttributeFactory.create(getReader(), cb.toString());

                //special method invoking
                if (attr instanceof CodeAttribute) {
                    ((CodeAttribute) attr).linkToConstantPool(constantPoolObject);
                }

                if (attr != null) {
                    attr.stuffing();
                    attributes.add(attr);
                } else {
                    u4 customAttributeLength = read4Bytes();
                    customeAttribute = new u1[(int) customAttributeLength.getValue()];
                    for (int m = 0; m < customeAttribute.length; m++) {
                        customeAttribute[m] = read1Byte();
                    }
                }
            } else {
                throw new RuntimeException("class load exception");
            }
        }
    }

    public ArrayList<Attribute> getAttributes() {
        return attributes;
    }

    @Override
    public String toString() {
        return attributes.toString();
    }
}
