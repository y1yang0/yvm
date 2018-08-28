package ydk.test;
import ydk.lang.IO;

class FieldBase{
    public int k = 1;
    public double d = 3.14;
    protected char c = 'F';
}
public class FieldAccess extends FieldBase {
    private int k;
    private double d;
    private char c;
    FieldAccess(){
        this.k = 2;
        this.d = 1024.56;
        this.c = 'Q';
    }

    public static void main(String[] args){
        FieldAccess fa = new FieldAccess();
        IO.print(fa.toString());
    }

    
    public String toString() {
        return  "FieldAccess{" +
                "k=" + k +
                ", d=" + d +
                ", c=" + c +
                "}\nBase{k=" + super.k +
                ", d=" + super.d +
                ", c=" + super.c +
                '}';
    }
}
