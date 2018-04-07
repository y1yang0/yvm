package ydk.test;

import ydk.lang.IO;

public class ObjectArrayTest {
    public static void main(String[] args){
        String[] strArr = new String[1024];
        for(int i=0;i<strArr.length;i++){
            strArr[i] = "hello world " + i;
        }
        for(int i=0;i<strArr.length;i++){
            IO.print(strArr[i]);
        }
    }
}
