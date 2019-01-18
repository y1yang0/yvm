package ydk.test;

import ydk.lang.IO;

public
class StaticVar {
private
    static int a = 123;

    static class StaticVarInner {
    public
        static int a = 4312;
    }

    public static String b = "predefined";

    static class StaticVarAnalogous extends StaticVarInner {
    public
        void getUpperStatic() { IO.print(a); }
    }

    public static void
    main(String[] args) {
        IO.print(StaticVar.a);
        IO.print(StaticVar.b);
        IO.print(StaticVarInner.a);
        new StaticVarAnalogous().getUpperStatic();
    }
}
