package yvm.adt;

public class Tuple3<_TypeA, _TypeB, _TypeC> {
    private _TypeA a;
    private _TypeB b;
    private _TypeC c;

    public Tuple3(_TypeA a, _TypeB b, _TypeC c) {
        this.a = a;
        this.b = b;
        this.c = c;
    }


    public _TypeA get1Placeholder() {
        return a;
    }

    public _TypeB get2Placeholder() {
        return b;
    }

    public _TypeC get3Placeholder() {
        return c;
    }


}
