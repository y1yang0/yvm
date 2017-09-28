package rtstruct;

public class YArray implements Initializable {
    private int dimension;
    private Object[] array;

    public YArray(int dimension) {
        array = new Object[dimension];
    }

    public int getDimension() {
        return dimension;
    }

    public Object get(int index) {
        return array[index];
    }

    public void set(int index, Object value) {
        array[index] = value;
    }

    @Override
    public void init() {
        for (int i = 0; i < dimension; i++) {
            array[i] = null;
        }
    }
}
