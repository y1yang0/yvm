package yvm.auxil;

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

    public static boolean inRange(Object[] arr, int index) {
        return index >= 0 && arr.length > index;
    }

    public static boolean isClass(Class x) {
        return x.isLocalClass() || x.isMemberClass() || x.isAnonymousClass();
    }

    public static boolean isSameClass(Class a, Class b) {
        if (isClass(a) && isClass(b)) {
            if (a == b && a.getClassLoader() == b.getClassLoader()) {
                return true;
            }
        }
        return false;
    }

    public static boolean isInterface(Class x) {
        return x.isInterface();
    }

    public static boolean isArray(Class x) {
        return x.isArray();
    }
}
