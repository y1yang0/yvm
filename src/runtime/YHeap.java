package runtime;

import java.util.ArrayList;

public class YHeap {
    private ArrayList<YArray> arrayArea;
    private ArrayList<YObject> objectArea;

    public YHeap() {
        arrayArea = new ArrayList<>();
        objectArea = new ArrayList<>();
    }

    public void addToArrayArea(YArray array) {
        arrayArea.add(array);
    }

    public void addToObjectArea(YObject object) {
        objectArea.add(object);
    }
}
