package rtstruct;

import java.util.ArrayList;

public class YHeap {
    private ArrayList<Object[]> arrayArea;
    private ArrayList<Object> objectArea;

    public YHeap() {
        arrayArea = new ArrayList<>();
        objectArea = new ArrayList<>();
    }

    public void addToArrayArea(Object[] array) {
        arrayArea.add(array);
    }

    public void addToObjectArea(Object object) {
        objectArea.add(object);
    }
}
