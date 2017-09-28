package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import ycloader.adt.field.FieldInfo;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.dataobject.FieldObject;
import yvm.adt.DebugExtension;
import yvm.adt.Resolvable;
import yvm.adt.Tuple4;
import yvm.constant.MetaClassConstant;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MetaClassField implements Resolvable<FieldObject>, DebugExtension {
    private
    Map<
            Integer,                    //constant pool index of this field
            Tuple4<                     //
                    String,             //field name
                    String,             //field descriptor
                    Integer,            //field access flags
                    Attribute[]>        //related attributes
            > fields;

    private MetaClass meta;

    public MetaClassField(MetaClass meta) {
        fields = new HashMap<>();
        this.meta = meta;
    }

    public Map getFields() {
        return fields;
    }

    @Override
    public void resolve(FieldObject r, ConstantPoolObject cp) {
        ArrayList<FieldInfo> fs = r.getRawData();
        for (int i = 0; i < r.getFieldCount(); i++) {
            int fieldNameIndex = fs.get(i).nameIndex.getValue();//constant pool index,it's necessary for code execution engine later
            int fieldDescriptorIndex = fs.get(i).descriptorIndex.getValue();
            String fieldName = cp.at(fieldNameIndex).toString();
            String fieldDescriptor = cp.at(fieldDescriptorIndex).toString();

            int accessFlag = fs.get(i).accessFlags.getValue();

            //if it's a static variable
            if ((accessFlag & MetaClassConstant.Access.ACC_STATIC) == 8) {
                meta.addStaticVariables(fieldNameIndex, new Tuple4<>(fieldName, fieldDescriptor, accessFlag, fs.get(i).attributes));
            } else {
                fields.put(fieldNameIndex, new Tuple4<>(fieldName, fieldDescriptor, accessFlag, fs.get(i).attributes));
            }
            //todo:optimize attribute[] to more concrete information[enhance]
            //todo:just we can merge synthetic and deprecated so on to one byte[enhance]
        }
    }

    @Override
    public synchronized void debug() {
        meta.getStaticVariable().forEach((A, B) -> {
            System.out.println("static index:" + A + "\n" +
                    "static field name:" + B.get1Placeholder() + "\n" +
                    "static field descriptor:" + B.get2Placeholder() + "\n");
        });

        fields.forEach((A, B) -> {
            System.out.println("index:" + A + "\n" +
                    "field name:" + B.get1Placeholder() + "\n" +
                    "field descriptor:" + B.get2Placeholder() + "\n" +
                    "field access flag:" + B.get3Placeholder() + "\n");
        });
    }
}
