package yvm.auxil;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Peel {
    public static String getArrayComponent(String qualifiedName) {
        Pattern pattern = Pattern.compile("\\[*([B|C|D|F|I|J|S|Z]|L(.*);){1}");
        Matcher matcher = pattern.matcher(qualifiedName);
        if (matcher.find()) {
            return matcher.group(2);
        }
        return null;
    }

    public static int getArrayDimension(String qualifiedName) {
        for (int i = 0; i < qualifiedName.length(); i++) {
            if (qualifiedName.charAt(i) != '[') {
                return i;
            }
        }
        return -1;
    }
}
