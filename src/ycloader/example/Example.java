package ycloader.example;

import java.util.ArrayList;

interface IA {
    void ia();
}

interface IB {
    void ib();
}

public class Example implements IA, IB {
    public static final int CONST_INT = 1024;
    public static int aaa = 16;
    public static int bbb = 17;
    private final String constStr = "this_is_a_long literal string";
    private long this_is_a_long;
    private double this_is_a_double;
    private int this_is_a_int;
    private String this_is_a_str = "你好，世界";
    @Deprecated
    private float this_is_a_float = 3.14F;
    private ArrayList<IA> ialist = new ArrayList<>();

    private Example(int argc, double argb, long arga, float argd, ArrayList<IA> ialist) {
        this.this_is_a_long = arga;
        this.this_is_a_double = argb;
        this.this_is_a_int = argc;
        this_is_a_float = argd;
        this.ialist = ialist;
    }

    public static void main(String[] args) {
        Example e = new Example(1, 15060949.6, 31415, 63.6F, new ArrayList<>());
        e.print();
        e.ia();
        e.ib();
    }

    @Deprecated
    public void print() {
        System.out.println(this_is_a_long + this_is_a_double + this_is_a_int);
        System.out.println(this_is_a_str);
        System.out.println(constStr);
    }

    long test(int a, int b) {
        long result = 0L;
        for (int i = 0; i < a + b; i++) {
            result >>= a + b + 3;
        }
        return result;
    }

    @Override
    public void ia() {
        System.out.println("this_is_a_long");
    }

    @Override
    public void ib() {
        System.out.println("this_is_a_double");
    }

    private class InnerClass {
    }

}
