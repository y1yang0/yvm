package ycloader.factory;

import ycloader.ClassFileReader;
import ycloader.adt.attribute.typeannotation.*;
import ycloader.exception.ClassLoadingException;

public class TargetFactory {
    public static Target create(ClassFileReader reader, int targetType) throws ClassLoadingException {
        if (targetType == 0x00) {
            return new TypeParameterTarget(reader);
        } else if (targetType == 0x01) {
            return new TypeParameterTarget(reader);
        } else if (targetType == 0x10) {
            return new SuperTypeTarget(reader);
        } else if (targetType == 0x11) {
            return new TypeParameterBoundTarget(reader);
        } else if (targetType == 0x12) {
            return new TypeParameterBoundTarget(reader);
        } else if (targetType == 0x13) {
            return new EmptyTarget(reader);
        } else if (targetType == 0x14) {
            return new EmptyTarget(reader);
        } else if (targetType == 0x15) {
            return new EmptyTarget(reader);
        } else if (targetType == 0x16) {
            return new FormalParameterTarget(reader);
        } else if (targetType == 0x17) {
            return new ThrowsTarget(reader);
        } else if (targetType == 0x40) {
            return new LocalVarTarget(reader);
        } else if (targetType == 0x41) {
            return new LocalVarTarget(reader);
        } else if (targetType == 0x42) {
            return new CatchTarget(reader);
        } else if (targetType == 0x43) {
            return new OffsetTarget(reader);
        } else if (targetType == 0x44) {
            return new OffsetTarget(reader);
        } else if (targetType == 0x45) {
            return new OffsetTarget(reader);
        } else if (targetType == 0x46) {
            return new OffsetTarget(reader);
        } else if (targetType == 0x47) {
            return new TypeArgumentTarget(reader);
        } else if (targetType == 0x48) {
            return new TypeArgumentTarget(reader);
        } else if (targetType == 0x49) {
            return new TypeArgumentTarget(reader);
        } else if (targetType == 0x4A) {
            return new TypeArgumentTarget(reader);
        } else if (targetType == 0x4B) {
            return new TypeArgumentTarget(reader);
        } else {
            throw new ClassLoadingException("invalid target type found");
        }
    }
}
