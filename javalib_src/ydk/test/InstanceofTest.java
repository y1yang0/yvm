package ydk.test;

import ydk.lang.IO;

public class InstanceofTest {
    public static class A{
    }
    public static class B extends A {
    }

    public static void main(String[] args) {
        A instance= new B();
        if(instance instanceof B){
            IO.print("instance type is class B");
        }
    }
}