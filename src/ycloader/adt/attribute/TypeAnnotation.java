package ycloader.adt.attribute;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.attribute.typeannotation.Target;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.exception.ClassLoadingException;
import ycloader.factory.TargetFactory;

import java.io.IOException;

public class TypeAnnotation extends BytesReaderProxy implements Stuffable, Verifiable {
    private u1 targetType;
    private Target targetInfo;
    private TypePath targetPath;
    private u2 typeIndex;
    private u2 numElementValuePairs;
    private ElementValuePair[] elementValuePairs;

    public TypeAnnotation(ClassFileReader reader) {
        super(reader);
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        targetType = read1Byte();

        targetInfo = TargetFactory.create(getReader(), targetType.getValue());
        targetInfo.stuffing();

        targetPath = new TypePath();
        targetPath.stuffing();

        typeIndex = read2Bytes();
        numElementValuePairs = read2Bytes();
        elementValuePairs = new ElementValuePair[numElementValuePairs.getValue()];
        for (int i = 0; i < numElementValuePairs.getValue(); i++) {
            ElementValuePair evp = new ElementValuePair(getReader());
            evp.stuffing();
            elementValuePairs[i] = evp;
        }
    }

    @Override
    public int getActualBytes() {
        int res = 1;
        res += targetInfo.getActualBytes();
        res += targetPath.getActualBytes();
        res += 4;
        for (ElementValuePair evp : elementValuePairs) {
            res += evp.getActualBytes();
        }
        return res;
    }

    class TypePath implements Stuffable, Verifiable {
        u1 pathLength;
        PathEntry[] path;

        @Override
        public void stuffing() throws IOException {
            pathLength = read1Byte();
            for (int i = 0; i < pathLength.getValue(); i++) {
                PathEntry pe = new PathEntry();
                pe.stuffing();
                path[i] = pe;
            }
        }

        public int getActualBytes() {
            int res = 1;
            for (PathEntry pe : path) {
                res += pe.getActualBytes();
            }
            return res;
        }

        class PathEntry implements Stuffable, Verifiable {
            u1 typePathKind;
            u1 typeArgumentIndex;

            public void stuffing() throws IOException {
                typePathKind = read1Byte();
                typeArgumentIndex = read1Byte();
            }

            public int getActualBytes() {
                return 2;
            }
        }
    }
}
