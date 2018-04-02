package ydk.test;

class WithReasonException extends Throwable{
    public WithReasonException(){ super(); }
    public WithReasonException(String msg){ super(msg); }
}
class WithoutReasonException extends Throwable{
    public WithoutReasonException(){ super(); }
    public WithoutReasonException(String msg){ super(msg); }
}

public class StackTraceTest {
    public static void main(String[] args) throws WithoutReasonException {
        func1();
    }
    private static void func1() throws WithoutReasonException { func2(); }
    private static void func2() throws WithoutReasonException { func3(); }
    private static void func3() throws WithoutReasonException { func4(); }
    private static void func4() throws WithoutReasonException { func5(); }
    private static void func5() throws WithoutReasonException { func6(); }
    private static void func6() throws WithoutReasonException { func7(); }
    private static void func7() throws WithoutReasonException {
        throw new WithoutReasonException("This exception was hidden on the deep calling chain");
    }
}
