package ydk.test;

import ydk.lang.IO;

class GenericException extends Throwable{
    public GenericException(){
        super();
    }
    public GenericException(String msg){
        super(msg);
    }
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

    public static void throwException2() throws GenericException {
        IO.print("deeply exception occurred\n");
        throw new GenericException("this exception hidden deeply");
    }
    public static void unhandledExceptionDeep3() throws GenericException{
        throw new GenericException("deep exception thrown");
    }
    public static void unhandledExceptionDeep2() throws GenericException{
        unhandledExceptionDeep3();
    }
    public static void unhandledExceptionDeep1() throws GenericException{
        unhandledExceptionDeep2();
    }
    public static void unhandledException() throws GenericException{
        unhandledExceptionDeep1();
    }

    public static void main(String[] args) throws GenericException {
        IO.print("catchIt()\n");
        catchIt();
        IO.print("throwAndCatch()\n");
        throwAndCatch();
        IO.print("nestException\n");
        nestException();
        IO.print("throwException2()\n");
        try{
            throwException2();
        }catch (GenericException e){
            IO.print("handled on main()\n");
        }
        IO.print("normal executing\n");
        unhandledException();
    }
}
