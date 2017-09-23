package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.constantpool.AbstractConstantPool;
import ycloader.adt.constantpool.ConstantUtf8Info;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.adt.u4;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.exception.ClassLoadingException;
import ycloader.factory.AttributeFactory;

import java.io.IOException;

public class CodeAttribute extends Attribute {
    public u2 maxStack;
    public u2 maxLocals;
    public u1[] code;
    public u2 exceptionTableLength;
    public ExceptionTable[] table;
    private u4 codeLength;
    private u2 attributesCount;
    private Attribute[] attributes;
    private u1[] customeAttribute;
    private ConstantPoolObject constantPoolObject;

    public CodeAttribute(ClassFileReader reader) {
        super(reader);
    }

    /**
     * this is a special method in all attributes class
     * code attribute may has other attribute, so we must
     * to check if it's a valid attribute before ready next
     * bytes;
     *
     * @param constantPoolObject
     */
    public void linkToConstantPool(ConstantPoolObject constantPoolObject) {
        this.constantPoolObject = constantPoolObject;
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        attributeLength = read4Bytes();
        maxStack = read2Bytes();
        maxLocals = read2Bytes();

        codeLength = read4Bytes();
        if (codeLength.getValue() < 0 || codeLength.getValue() > 65536) {
            throw new ClassLoadingException("code length out of limitation");
        }
        code = new u1[(int) codeLength.getValue()];
        for (int i = 0; i < codeLength.getValue(); i++) {
            code[i] = read1Byte();
        }

        exceptionTableLength = read2Bytes();
        table = new ExceptionTable[exceptionTableLength.getValue()];
        for (int k = 0; k < exceptionTableLength.getValue(); k++) {
            ExceptionTable et = new ExceptionTable();
            et.stuffing();
            table[k] = et;
        }
        attributesCount = read2Bytes();

        //extra check
        if (attributesCount.getValue() > 0 && constantPoolObject == null) {
            throw new RuntimeException("no ycloader.constant pool linked");
        }

        attributes = new Attribute[attributesCount.getValue()];

        for (int t = 0; t < attributesCount.getValue(); t++) {
            u2 attributeNameIndex = read2Bytes();
            AbstractConstantPool cb = constantPoolObject.at(attributeNameIndex.getValue());
            //if it's not a ycloader.constant utf-8 info structure then throws an ycloader.exception

            if (cb instanceof ConstantUtf8Info) {
                Attribute codeAttr = AttributeFactory.create(getReader(), cb.toString());

                //special method invoking
                if (codeAttr instanceof CodeAttribute) {
                    ((CodeAttribute) codeAttr).linkToConstantPool(constantPoolObject);
                }

                if (codeAttr != null) {
                    codeAttr.stuffing();
                    attributes[t] = codeAttr;
                } else {
                    u4 customAttributeLength = read4Bytes();
                    customeAttribute = new u1[(int) customAttributeLength.getValue()];
                    for (int m = 0; m < customeAttribute.length; m++) {
                        customeAttribute[m] = read1Byte();
                    }
                }
            } else {
                throw new ClassLoadingException(t + "," + "class load exception");
            }
        }
    }

    @Override
    public int getActualBytes() {
        int res = 8;
        res += code.length;
        res += 2;
        for (ExceptionTable et : table) {
            res += et.getActualBytes();
        }
        res += 2;
        if (customeAttribute == null || customeAttribute.length == 0) {
            for (Attribute ab : attributes) {
                res += ab.getActualBytes();
                //it's important
                //other attribute many existed in code attribute structure
                //so we need to add extra 6 bytes representing attributeNameIndex
                //attributeLength
                res += 6;
            }
        } else {
            res += customeAttribute.length;
        }
        return res;
    }

    public class ExceptionTable {
        public u2 startPc;
        public u2 endPc;
        public u2 handlerPc;
        public u2 catchType;

        void stuffing() throws IOException {
            startPc = read2Bytes();
            endPc = read2Bytes();
            handlerPc = read2Bytes();
            catchType = read2Bytes();
        }

        int getActualBytes() {
            return 8;
        }
    }
}
