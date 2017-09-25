package yvm.auxil;

public class Common {
    public static <_T> boolean isNull(_T obj) {
        return obj == null;
    }

    public static String rawArrayToString(int[] arr, char delimiter) {
        StringBuilder sb = new StringBuilder();
        if (arr.length == 0) {
            return "";
        }

        for (int x : arr) {
            sb.append("#").append(x).append(delimiter);
        }
        sb.deleteCharAt(sb.length() - 1);
        return sb.toString();
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
