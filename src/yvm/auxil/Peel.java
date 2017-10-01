package yvm.auxil;

import runtime.rtexception.VMExecutionException;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Peel {
    public static void main(String[] args) {
        peelMethodDescriptorParameter("(BBB)V");
        peelMethodDescriptorParameter("()Ljava/lang/String;");
        peelMethodDescriptorParameter("(Ljava/lang/String;)B");
        peelMethodDescriptorParameter("(I)Ljava/lang/String;");
        peelMethodDescriptorParameter("(J)V");
        peelMethodDescriptorParameter("()[I");
        peelMethodDescriptorParameter("(I[[DJFLjava/util/ArrayList;Ljava/util/ArrayList;Ljava/lang/String;)V");
        peelMethodDescriptorParameter("([ID[[[JFLjava/util/ArrayList;)V");
        peelMethodDescriptorParameter("([II[II)[I");
        peelMethodDescriptorParameter("([I[I[IIJ[I)[I");
        peelMethodDescriptorParameter("()Ljava/math/BigInteger;");
        peelMethodDescriptorParameter("(Ljava/math/BigInteger;Ljava/math/BigInteger;)Ljava/math/BigInteger;");
        peelMethodDescriptorParameter("(Ljava/math/BigInteger;Ljava/math/BigInteger;)Ljava/math/BigInteger;");
        peelMethodDescriptorParameter("(Ljava/math/MutableBigInteger;Ljava/math/MutableBigInteger;Z)Ljava/math/MutableBigInteger;");
        System.out.println(peelFieldDescriptor("I[[DJFLjava/util/ArrayList;Ljava/util/ArrayList;[[Ljava/lang/String;"));
        System.out.println(peelFieldDescriptor("[ID[[[JFLjava/util/ArrayList;"));
        System.out.println(peelFieldDescriptor("BBB"));
        System.out.println(peelFieldDescriptor(""));
        System.out.println(peelFieldDescriptor("[I[I[IIJ[I"));
        System.out.println(peelFieldDescriptor("[[Ljava/math/MutableBigInteger;Ljava/math/MutableBigInteger;Z"));
        System.out.println(peelFieldDescriptor("I[[DJFLjava/util/ArrayList;[[Ljava/util/ArrayList;Ljava/lang/String;"));
        System.out.println(peelFieldDescriptor("[II[II"));
        System.out.println(peelFieldDescriptor("V"));
        System.out.println(0x1234 & 0x0200);
    }

    public static String[] peelMethodDescriptorParameter(String methodDescriptor) {
        Pattern pattern = Pattern.compile("\\(((\\[*[BCDFIJSZ]|\\[*L.*;)*)\\)(\\[*[VBCDFIJSZ]|\\[*L.*;){1}");
        Matcher matcher = pattern.matcher(methodDescriptor);
        if (matcher.find()) {
            return new String[]{
                    matcher.group(1),               //method parameter
                    matcher.group(3)};               //method return type
        } else {
            throw new VMExecutionException("invalid method descriptor :" + methodDescriptor);
        }
    }

    public static ArrayList<String> peelFieldDescriptor(String qualifiedClassNameSequence) {
        if (qualifiedClassNameSequence.equals("")) {
            return null;
        }

        ArrayList<String> nameSequences = new ArrayList<>();

        Pattern pattern = Pattern.compile("(\\[*[BCDFIJSZV])|(\\[*(?=L)[A-Za-z0-9$/]*)(?:;)");
        Matcher matcher = pattern.matcher(qualifiedClassNameSequence);
        while (matcher.find()) {
            String primitiveType = matcher.group(1);
            String referenceType = matcher.group(2);
            if (primitiveType != null) {
                switch (primitiveType.replace("[", "")) {
                    case "B":
                        nameSequences.add(primitiveType.replace("B", "java/lang/Byte"));
                        break;
                    case "C":
                        nameSequences.add(primitiveType.replace("C", "java/lang/Character"));
                        break;
                    case "D":
                        nameSequences.add(primitiveType.replace("D", "java/lang/Double"));
                        break;
                    case "F":
                        nameSequences.add(primitiveType.replace("F", "java/lang/Float"));
                        break;
                    case "I":
                        nameSequences.add(primitiveType.replace("I", "java/lang/Integer"));
                        break;
                    case "J":
                        nameSequences.add(primitiveType.replace("J", "java/lang/Long"));
                        break;
                    case "S":
                        nameSequences.add(primitiveType.replace("S", "java/lang/Short"));
                        break;
                    case "Z":
                        nameSequences.add(primitiveType.replace("Z", "java/lang/Boolean"));
                        break;
                    case "V":
                        nameSequences.add("java/lang/Void");
                        break;
                }
            } else if (referenceType != null) {
                nameSequences.add(referenceType.replaceFirst("L", ""));
            }
        }
        return nameSequences;
    }

    public static int getArrayDimension(String arrayDesc) {
        int count = 0;
        for (int i = 0; i < arrayDesc.length(); i++) {
            if (arrayDesc.charAt(i) == '[')
                count++;
        }
        return count;
    }

}
