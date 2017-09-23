package ycloader.adt.attribute.typeannotation;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.attribute.Verifiable;

public abstract class Target extends BytesReaderProxy implements Stuffable, Verifiable {
    Target(ClassFileReader reader) {
        super(reader);
    }
}
