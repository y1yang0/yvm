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

    public static String peelFieldDescriptor(String qualifiedClassName) {
        qualifiedClassName = qualifiedClassName.replaceAll("^\\[*L{0,1}", "");
        qualifiedClassName = qualifiedClassName.replaceAll(";", "");
        switch (qualifiedClassName) {
            case "B":
                return "java/lang/Byte";
            case "C":
                return "java/lang/Character";
            case "D":
                return "java/lang/Double";
            case "F":
                return "java/lang/Float";
            case "I":
                return "java/lang/Integer";
            case "J":
                return "java/lang/Long";
            case "S":
                return "java/lang/Short";
            case "Z":
                return "java/lang/Boolean";
            default:
                return qualifiedClassName;
        }
    }

    public static void main(String[] args) {
        System.out.println(peelFieldDescriptor("java/lang/ClassLoader.class"));
    }

    public String[] peelMethodDescriptorParameter(String methodDescriptor) {
        int leftBracket = methodDescriptor.indexOf('(');
        int rightBracket = methodDescriptor.indexOf(')');
        String parameterDescriptor = methodDescriptor.substring(leftBracket, rightBracket);
        Pattern pattern = Pattern.compile("\\(\\)(V)|([B|C|D|F|I|J|S|Z]{1})|(L(.*);)|(\\[+([B|C|D|F|I|J|S|Z]{1}|L(.*);))");

    }
}
