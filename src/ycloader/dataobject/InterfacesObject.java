package ycloader.dataobject;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.u2;

import java.io.IOException;
import java.util.ArrayList;


public class InterfacesObject extends BytesReaderProxy implements Stuffable {
    private int interfacesCount;
    private ArrayList<u2> interfaceList;

    public InterfacesObject(ClassFileReader reader, int interfacesCount) {
        super(reader);
        this.interfacesCount = interfacesCount;
        interfaceList = new ArrayList<>();
    }

    public ArrayList<u2> getInterfaces() {
        return interfaceList;
    }

    @Override
    public void stuffing() throws IOException {
        for (int i = 0; i < interfacesCount; i++) {
            u2 x = read2Bytes();
            interfaceList.add(x);
        }
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (u2 anInterfaceList : interfaceList) {
            sb.append(anInterfaceList.getValue()).append("@ConstPoolTable");
            sb.append(",");
        }
        if (sb.length() != 1) {
            sb.delete(sb.length() - 1, sb.length());
        }
        sb.append("]");
        return sb.toString();
    }
}
