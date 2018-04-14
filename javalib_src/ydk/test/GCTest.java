package ydk.test;

public class GCTest {
    public static void produceGarbage(){
        for(int i=0;i<100000;i++){
            new Object();
        }
    }
    public static void produceArrayGarbage(){
        for(int i=0;i<100000;i++) {
            Object[] unused = new Object[100];
        }
    }
    public static void main(String[] args){
        produceGarbage();
        produceArrayGarbage();
    }
}
