package ycloader.adt.attribute;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.exception.ClassLoadingException;

import java.io.IOException;

interface Value extends Stuffable, Verifiable {
}

class ElementValue extends BytesReaderProxy implements Stuffable, Verifiable {
    private u1 tag;
    private Value value;

    ElementValue(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        tag = read1Byte();
        if (tag.equal('B') || tag.equal('C') || tag.equal('D') || tag.equal('F') || tag.equal('I') ||
                tag.equal('J') || tag.equal('S') || tag.equal('Z') || tag.equal('s')) {
            value = new ConstValue();
        } else if (tag.equal('e')) {
            value = new EnumValue();
        } else if (tag.equal('c')) {
            value = new ClassInfoValue();
        } else if (tag.equal('@')) {
            value = new AnnotationValue();
        } else if (tag.equal('[')) {
            value = new ArrayValue();
        } else {
            throw new ClassLoadingException("invalid annotation tag type found");
        }
        value.stuffing();
    }

    @Override
    public int getActualBytes() {
        return 1 + value.getActualBytes();
    }

    class ConstValue implements Value {
        u2 constValueIndex;

        @Override
        public void stuffing() throws IOException {
            constValueIndex = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 2;
        }
    }

    class EnumValue implements Value {
        u2 typeNameIndex;
        u2 constNameIndex;

        @Override
        public void stuffing() throws IOException {
            typeNameIndex = read2Bytes();
            constNameIndex = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 4;
        }
    }

    class ClassInfoValue implements Value {
        u2 classInfoIndex;

        @Override
        public void stuffing() throws IOException {
            classInfoIndex = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 2;
        }
    }

    class AnnotationValue implements Value {
        Annotation annotationValue;

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            Annotation annotation = new Annotation(getReader());
            annotation.stuffing();
            annotationValue = annotation;
        }

        @Override
        public int getActualBytes() {
            return annotationValue.getActualBytes();
        }
    }

    class ArrayValue implements Value {
        u2 numValues;
        ElementValue[] elementValues;

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            numValues = read2Bytes();
            elementValues = new ElementValue[numValues.getValue()];
            for (int i = 0; i < numValues.getValue(); i++) {
                ElementValue ev = new ElementValue(getReader());
                ev.stuffing();
                elementValues[i] = ev;
            }
        }

        @Override
        public int getActualBytes() {
            int res = 2;
            for (ElementValue ev : elementValues) {
                res += ev.getActualBytes();
            }
            return res;
        }
    }
}

class ElementValuePair extends BytesReaderProxy implements Stuffable, Verifiable {
    private u2 elementNameIndex;
    private ElementValue value;

    public ElementValuePair(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        elementNameIndex = read2Bytes();
        ElementValue elementValue = new ElementValue(getReader());
        elementValue.stuffing();
        value = elementValue;
    }

    @Override
    public int getActualBytes() {
        return 2 + value.getActualBytes();
    }
}

public class Annotation extends BytesReaderProxy implements Stuffable, Verifiable {
    private u2 typeIndex;
    private u2 numElementValuePairs;
    private ElementValuePair[] elementValuePairs;

    public Annotation(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        typeIndex = read2Bytes();
        numElementValuePairs = read2Bytes();
        elementValuePairs = new ElementValuePair[numElementValuePairs.getValue()];
        for (int i = 0; i < numElementValuePairs.getValue(); i++) {
            ElementValuePair elementValuePair = new ElementValuePair(getReader());
            elementValuePair.stuffing();
            elementValuePairs[i] = elementValuePair;
        }
    }

    @Override
    public int getActualBytes() {
        int res = 4;
        for (ElementValuePair evp : elementValuePairs) {
            res += evp.getActualBytes();
        }
        return res;
    }
}
