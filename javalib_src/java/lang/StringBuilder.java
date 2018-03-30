package java.lang;

public class StringBuilder {
    private char value[];

    public StringBuilder(){
        value = null;
    }

    public StringBuilder(char c) {
        this.value = new char[1];
        this.value[0] = c;
    }

    public StringBuilder(int v) {
        this.value = new char[1];
        this.value[0] = (char)v;
    }

    public StringBuilder(char[] strArr) {
        this.value = new char[strArr.length];
        for(int i = 0; i < strArr.length; ++i) {
            this.value[i] = strArr[i];
        }

    }
    public native StringBuilder append(String var1);
    public native StringBuilder append(int var1);
    public native StringBuilder append(char var1);
    public native String toString();
}
