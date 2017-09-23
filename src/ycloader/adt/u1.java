package ycloader.adt;

public class u1 {
    private int value;

    public u1(int v) {
        value = v;
    }

    public u1(u1 b) {
        if (b.getValue() >= 0) {
            value = b.getValue();
        } else {
            value = 127 + (128 + b.getValue()) + 1;
        }
    }

    public boolean equal(int x) {
        return x == value;
    }

    public boolean equal(byte x) {
        return x == (byte) value;
    }

    public boolean equal(char c) {
        return (char) value == c;
    }

    public int getValue() {
        return value;
    }
}
