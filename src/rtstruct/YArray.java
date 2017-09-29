package rtstruct;

public class YArray extends YObject{
    private int length;

    public YArray(int dimension) {
        super(dimension);
        this.length = dimension;
    }

    public int getLength() {
        return length;
    }

    public YObject get(int index) {
        return getArrayComponent(index);
    }

    public void set(int index, YObject value) {
        setArrayComponent(index,value);
    }
}
