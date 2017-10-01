package common;

public class Tuple5<_TypeA, _TypeB, _TypeC, _TypeD, _TypeE> {
    private _TypeA a;
    private _TypeB b;
    private _TypeC c;
    private _TypeD d;
    private _TypeE e;

    public Tuple5(_TypeA a, _TypeB b, _TypeC c, _TypeD d, _TypeE e) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
        this.e = e;
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

}
