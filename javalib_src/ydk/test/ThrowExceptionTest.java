package ydk.test;

import ydk.lang.IO;

class GenericException extends Throwable{

}

public class ThrowExceptionTest {
    public static void bar() throws GenericException {
        IO.print("before throwing exception\n");
        throw new GenericException();
    }

    public static void foo() throws GenericException {
        bar();
        IO.print("should not reach here");
    }

    public static void catchIt(){
        try {
            bar();
        } catch (GenericException e) {
            IO.print("exception had been caught\n");
        }
    }

    public static void throwAndCatch(){
        try{
            throw new GenericException();
        }catch (GenericException e){
            IO.print("exception had been caught\n");
        }
    }

    public static void nestException(){
        try{
            IO.print("prelude");
            throw new GenericException();
        }catch (GenericException e){
            IO.print("nest exception had been caught\n");
        }finally {
            IO.print("print it whatever exception had been caught\n");
        }
    }

    public static void main(String[] args) throws GenericException {
        catchIt();
        throwAndCatch();
        nestException();
    }
}
