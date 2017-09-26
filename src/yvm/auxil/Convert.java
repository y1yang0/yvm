package yvm.auxil;

public class Convert {
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
}
