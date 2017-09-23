package ycloader.adt.attribute;

import ycloader.ClassFileReader;
import ycloader.adt.u2;
import ycloader.exception.ClassLoadingException;

import java.io.IOException;

public class RuntimeVisibleAnnotationsAttribute extends Attribute {
    private u2 numAnnotations;
    private Annotation[] annotations;

    public RuntimeVisibleAnnotationsAttribute(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        attributeLength = read4Bytes();
        numAnnotations = read2Bytes();

        annotations = new Annotation[numAnnotations.getValue()];
        for (int i = 0; i < numAnnotations.getValue(); i++) {
            Annotation a = new Annotation(getReader());
            a.stuffing();
            annotations[i] = a;

        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (Annotation a : annotations) {
            res += a.getActualBytes();
        }
        return res;
    }
}
