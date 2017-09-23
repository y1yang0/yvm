package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.CodeAttribute;
import ycloader.adt.field.FieldInfo;
import ycloader.adt.u1;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.dataobject.MethodObject;
import yvm.adt.Resolvable;
import yvm.adt.auxil.Tuple6;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class MetaClassMethod implements Resolvable<MethodObject> {
    private
    Map<Integer,                    //method index at constant pool
            Tuple6<                     //
                    String,                 //method name
                    String,                 //method descriptor
                    u1[],                   //method codes
                    StackRequirement,       //stack requirement for this method
                    ExceptionTable[],       //method exception tables,they are differ from checked exception in function signature
                    ArrayList<Attribute>    //method related attributes,it would be use for future vm version,there just ignore them
                    >> methods;

    public MetaClassMethod() {
        methods = new HashMap<>();
    }

    public synchronized void debug() {
        methods.forEach((A, B) -> {
            System.out.println("index:" + A + "\n" +
                    "name:" + B.get1Placeholder() + "\n" +
                    "descriptor:" + B.get2Placeholder() + "\n" +
                    "codes" + B.get3Placeholder() + "\n" +
                    "max stack:" + B.get4Placeholder().maxStack + "\n" +
                    "max locals:" + B.get4Placeholder().maxLocals + "\n" +
                    "exception table:" + B.get5Placeholder() + "\n" +
                    "attributes:" + B.get6Placeholder() + "\n"
            );
        });
    }

    public Tuple6 findMethod(String methodName) {
        for (Tuple6 t : methods.values()) {
            if (t.get1Placeholder().equals(methodName)) {
                return t;
            }
        }
        return null;
    }

    @Override
    public void resolve(MethodObject r, ConstantPoolObject cp) {
        ArrayList<FieldInfo> rmethods = r.getRawData();
        for (int i = 0; i < r.getFieldCount(); i++) {
            int methodNameIndex = rmethods.get(i).nameIndex.getValue();//constant pool index,it's necessary for code execution engine later
            int methodDescriptorIndex = rmethods.get(i).descriptorIndex.getValue();
            ArrayList<Attribute> attrs = r.getAttributes();

            StackRequirement sr = new StackRequirement();
            MetaClassMethod.ExceptionTable[] table = null;
            u1[] codes = null;
            for (Attribute attribute : attrs) {
                if (attribute instanceof CodeAttribute) {
                    sr.maxStack = ((CodeAttribute) attribute).maxStack.getValue();
                    sr.maxLocals = ((CodeAttribute) attribute).maxLocals.getValue();

                    table = new ExceptionTable[((CodeAttribute) attribute).exceptionTableLength.getValue()];
                    for (int k = 0; k < ((CodeAttribute) attribute).exceptionTableLength.getValue(); k++) {
                        CodeAttribute.ExceptionTable[] x = ((CodeAttribute) attribute).table;
                        /*table[k].startPC = ((CodeAttribute) attribute).table[k].startPc.getValue();
                        table[k].endPC = ((CodeAttribute) attribute).table[k].endPc.getValue();
                        table[k].handlePC = ((CodeAttribute) attribute).table[k].handlerPc.getValue();
                        table[k].catchTypeName = cp.getClassName(((CodeAttribute) attribute).table[k].catchType.getValue());
                    */
                    }
                    codes = ((CodeAttribute) attribute).code;

                }
            }
            methods.put(methodNameIndex,
                    new Tuple6<>(
                            cp.at(methodNameIndex).toString(),
                            cp.at(methodDescriptorIndex).toString(),
                            codes,
                            sr,
                            table,
                            attrs));
        }
    }

    public class StackRequirement {
        public int maxStack;
        public int maxLocals;
    }

    public class ExceptionTable {
        public int startPC;
        public int endPC;
        public int handlePC;
        public String catchTypeName;
    }
}
