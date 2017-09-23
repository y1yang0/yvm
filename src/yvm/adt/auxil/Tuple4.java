package yvm.adt.auxil;

public class Tuple4<_TypeA, _TypeB, _TypeC, _TypeD> {
    private _TypeA a;
    private _TypeB b;
    private _TypeC c;
    private _TypeD d;

    public Tuple4(_TypeA a, _TypeB b, _TypeC c, _TypeD d) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
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

    public _TypeD get4Placeholder() {
        return d;
    }
}

