package testcase;

public class CLinitTest {
    static {
        double x = 5 / 2;
        Object[] pp = new Object[14];
        double[] xx = new double[14];
        for (int i = 0; i < xx.length; i++) {
            xx[i] = 0.0000000000007 * xx[i] * (3 << 2);
        }
    }

    public static void main(String[] args) {
        CLinitTest e = new CLinitTest();
    }
}
