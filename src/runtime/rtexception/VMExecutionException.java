package runtime.rtexception;

public class VMExecutionException extends RuntimeException{
    public VMExecutionException(String message){
        super(message);
    }
}
