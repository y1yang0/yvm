package java.lang;


public class String {
    private char[] value;

    public String(char c) {
        this.value = new char[1];
        this.value[0] = c;
    }

    public String(int v) {
        this.value = new char[1];
        this.value[0] = (char)v;
    }

    public String(char[] strArr) {
        this.value = new char[strArr.length];

        for(int i = 0; i < strArr.length; ++i) {
            this.value[i] = strArr[i];
        }
    }
}
