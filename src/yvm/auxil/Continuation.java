package yvm.auxil;

public class Continuation {
    public static void ifNullThrowNullptrException(Object object){
        if(object==null){
            throw new NullPointerException("the object \""+ object.toString() + "\" is null");
        }
    }
}
