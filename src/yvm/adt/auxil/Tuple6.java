package yvm.adt.auxil;

public class Tuple6<_TypeA, _TypeB, _TypeC, _TypeD, _TypeE, _TypeF> {
    private _TypeA a;
    private _TypeB b;
    private _TypeC c;
    private _TypeD d;
    private _TypeE e;
    private _TypeF f;

    public Tuple6(_TypeA a, _TypeB b, _TypeC c, _TypeD d, _TypeE e, _TypeF f) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
        this.e = e;
        this.f = f;
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

    public _TypeE get5Placeholder() {
        return e;
    }

    public _TypeF get6Placeholder() {
        return f;
    }

}
