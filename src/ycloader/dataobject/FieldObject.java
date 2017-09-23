package ycloader.dataobject;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.field.FieldInfo;
import ycloader.exception.ClassLoadingException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;

public class FieldObject extends BytesReaderProxy implements Stuffable {
    private int fieldCount;
    private ArrayList<FieldInfo> fields;

    private ConstantPoolObject constantPoolObject;

    public FieldObject(ClassFileReader reader, ConstantPoolObject cpObject, int fieldCount) {
        super(reader);
        this.fieldCount = fieldCount;
        this.constantPoolObject = cpObject;
        fields = new ArrayList<>();
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        for (int i = 0; i < fieldCount; i++) {
            FieldInfo field = new FieldInfo(getReader(), constantPoolObject);
            field.stuffing();
            fields.add(field);
        }
    }

    public ArrayList<FieldInfo> getRawData() {
        return fields;
    }

    public int getFieldCount() {
        return fieldCount;
    }

    public ArrayList<Attribute> getAttributes() {
        ArrayList<Attribute> abList = new ArrayList<>();
        for (FieldInfo fi : fields) {
            if (fi.attributeCount.getValue() > 0) {
                Collections.addAll(abList, fi.attributes);
            }
        }
        return abList;
    }

    public ArrayList<String> getNames() {
        ArrayList<String> names = new ArrayList<>();
        for (FieldInfo field : fields) {
            names.add(constantPoolObject.at(field.nameIndex.getValue()).toString());
        }
        return names;
    }

    public ArrayList<String> getDescriptors() {
        ArrayList<String> descriptor = new ArrayList<>();
        for (FieldInfo field : fields) {
            descriptor.add(constantPoolObject.at(field.descriptorIndex.getValue()).toString());
        }
        return descriptor;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (FieldInfo field : fields) {
            sb.append("{name:");
            sb.append(constantPoolObject.at(field.nameIndex.getValue()));
            sb.append(" desc:");
            sb.append(constantPoolObject.at(field.descriptorIndex.getValue()));
            sb.append(" attr:");
            for (int k = 0; k < field.attributeCount.getValue(); k++) {
                sb.append("[");
                sb.append(field.attributes[k].getClass().getSimpleName());
                sb.append("]");
            }
            sb.append("}");
            sb.append(",");
        }
        if (sb.length() != 1) {
            sb.delete(sb.length() - 1, sb.length());
        }
        sb.append("]");
        return sb.toString();
    }
}
