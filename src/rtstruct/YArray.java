package rtstruct;

public class YArray<_ComponentType> implements Initializable {
    private int dimension;
    private Object[] array;
    private _ComponentType component;

    public YArray(int dimension, _ComponentType component) {
        array = new Object[dimension];
        this.component = component;
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
