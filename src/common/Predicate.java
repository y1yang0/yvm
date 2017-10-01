package common;

import runtime.YArray;

public class Predicate {
    public static <_T> boolean isNull(_T obj) {
        return obj == null;
    }

    public static boolean strEqual(String a, String b) {
        return a.equals(b);
    }

    public static boolean strNotEqual(String a, String b) {
        return !a.equals(b);
    }

    public static boolean inRange(YArray arr, int index) {
        return index >= 0 && arr.getLength() > index;
    }

    public static boolean isClass(Class x) {
        return x.isLocalClass() || x.isMemberClass() || x.isAnonymousClass();
    }

    public static boolean isCategory1ComputationalType(Object x) {
        return !isCategory2ComputationalType(x);
    }

    public static boolean isCategory2ComputationalType(Object x) {
        return x instanceof Double || x instanceof Long;
    }
}
