package ycloader.factory;

import ycloader.adt.attribute.stackmaptable.StackMapFrame;
import ycloader.adt.attribute.stackmaptable.StackMapTableAttribute;
import ycloader.exception.ClassLoadingException;

public class StackMapFrameFactory {
    private static boolean atRange(int num, int l, int r) {
        return num >= l && num <= r;
    }

    private static boolean atRange(int num, int l) {
        return num == l;
    }

    public static StackMapFrame create(StackMapTableAttribute outer, int n) throws ClassLoadingException {
        if (atRange(n, 0, 63)) {
            return outer.new SMF_SameFrame(n);
        } else if (atRange(n, 64, 127)) {
            return outer.new SMF_SameLocals1StackItemFrame(n);
        } else if (atRange(n, 247)) {
            return outer.new SMF_SameLocals1StackItemFrameExtended(n);
        } else if (atRange(n, 248, 250)) {
            return outer.new SMF_ChopFrame(n);
        } else if (atRange(n, 251)) {
            return outer.new SMF_SameFrameExtended(n);
        } else if (atRange(n, 252, 254)) {
            return outer.new SMF_AppendFrame(n);
        } else if (atRange(n, 255)) {
            return outer.new SMF_FullFrame(n);
        } else {
            throw new ClassLoadingException("stack map frame type " + n + " out of range frame type");
        }
    }
}