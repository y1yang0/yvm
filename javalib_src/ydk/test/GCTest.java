package ydk.test;

import ydk.lang.IO;

public class GCTest {
    private String[] strings = null;
    private int cnt = 0;
    private static final int MAX = 20000;

    public static void produceGarbage(){
        for(int i=0;i<10000;i++){
            new Object();
        }
    }
    public static void produceArrayGarbage(){
        for(int i=0;i<10000;i++) {
            Object[] unused = new Object[100];
        }
    }

    public void fullGC(){
        for(int i=0;i<MAX;i++){
           new Object();
        }
    }

    public void halfGC(){
        strings = new String[MAX/2];
        for(int i=0;i<MAX;i++){
            if(i%2==0){
                strings[cnt++] = "This is "+ i +" times to say hello to you\n";
            }
        }
        cnt=0;
    }
    public void print(){
        for(int i=0;i<MAX/2;i++){
            IO.print(strings[i]);
        }
    }

    public static void main(String[] args){
        //produceGarbage();
        //produceArrayGarbage();
        GCTest test = new GCTest();
        test.fullGC();
        test.fullGC();
        test.halfGC();
        test.halfGC();
        test.print();
    }
}
