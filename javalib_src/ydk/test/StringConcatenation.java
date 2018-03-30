package ydk.test;

import ydk.lang.IO;

public class StringConcatenation {
    static String constantValue = "constant value field";

    public static class A{
    }
    public static class B extends A{
    }
    public static void main(String[] args) {
        constantValue += " add new string";
        constantValue += " add again" + "add again\n";
        IO.print(constantValue);
    }
}
