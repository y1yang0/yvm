package ycloader.adt.constantpool;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;


abstract public class AbstractConstantPool extends BytesReaderProxy implements Stuffable {
    private int tableIndex;

    AbstractConstantPool(ClassFileReader reader) {
        super(reader);
    }

    public int getTableIndex() {
        return tableIndex;
    }

    public void setTableIndex(int index) {
        tableIndex = index;
    }

    @Override
    public String toString() {
        return this.getClass().getSimpleName();
    }
}
