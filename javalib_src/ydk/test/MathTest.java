package ydk.test;

import ydk.lang.IO;

public class MathTest {
    public static void generateRandomNumber(int n){
        for(int i=0;i<n;i++){
            IO.print((int)Math.random());
            IO.print(' ');
        }
        IO.print(' ');
    }
    public static void main(String[] args){
        generateRandomNumber(20);
    }
}
