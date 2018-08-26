package java.lang;

import ydk.lang.IO;

import java.lang.String;

public class Throwable {
    private final java.lang.String message;

    public Throwable(){
        message = null;
    }
    public Throwable(String str){
        message = str;
    }
    public void printStackTrace() {
        IO.print(message);
    }
    public static void main(String[] args){}
}
