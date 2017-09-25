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
}
