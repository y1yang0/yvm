package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.CodeAttribute;
import ycloader.adt.field.FieldInfo;
import ycloader.adt.u1;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.dataobject.MethodObject;
import yvm.adt.Resolvable;
import yvm.adt.Tuple6;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

class MethodAccessProperty {
    public static final int ACC_SYNCHRONIZED = 0x0020;
    public static final int ACC_STATIC = 0x0008;
    public static final int ACC_ABSTRACT = 0x0400;
    public static final int ACC_NATIVE = 0x0100;
    public static final int ACC_PROTECTED = 0x0004;
    public static final int ACC_PUBLIC = 0x0001;
    public static final int ACC_PRIVATE = 0x0002;
}

public class MetaClassMethod implements Resolvable<MethodObject> {
    private
    Map<Integer,                                //method index at constant pool
            Tuple6<                             //
                    String,                     //method name
                    String,                     //method descriptor
                    u1[],                       //method codes
                    StackRequirement,           //stack requirement for this method
                    ArrayList<ExceptionTable>,  //method exception tables,they are differ from checked exception in function signature
                    MethodExtension             //it would be change frequently, so there we create a flexible class to store data
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
        ArrayList<FieldInfo> allMethods = r.getRawData();
        for (int i = 0; i < r.getFieldCount(); i++) {

            int methodNameIndex = allMethods.get(i).nameIndex.getValue();//constant pool index,it's necessary for code execution engine later

            int methodDescriptorIndex = allMethods.get(i).descriptorIndex.getValue();

            MethodExtension extension = new MethodExtension();
            extension.attrs = r.getAttributes();
            extension.isSynchronized = getFlag(allMethods.get(i).accessFlags.getValue(), MethodAccessProperty.ACC_SYNCHRONIZED);
            extension.isStatic = getFlag(allMethods.get(i).accessFlags.getValue(), MethodAccessProperty.ACC_STATIC);
            extension.isAbstract = getFlag(allMethods.get(i).accessFlags.getValue(), MethodAccessProperty.ACC_ABSTRACT);
            extension.isNative = getFlag(allMethods.get(i).accessFlags.getValue(), MethodAccessProperty.ACC_NATIVE);
            extension.isProtected = getFlag(allMethods.get(i).accessFlags.getValue(), MethodAccessProperty.ACC_PROTECTED);
            StackRequirement sr = new StackRequirement();

            ArrayList<MetaClassMethod.ExceptionTable> table = new ArrayList<>();

            u1[] codes = null;

            for (Attribute attribute : extension.attrs) {
                if (attribute instanceof CodeAttribute) {
                    sr.maxStack = ((CodeAttribute) attribute).maxStack.getValue();
                    sr.maxLocals = ((CodeAttribute) attribute).maxLocals.getValue();

                    for (int k = 0; k < ((CodeAttribute) attribute).exceptionTableLength.getValue(); k++) {
                        MetaClassMethod.ExceptionTable et = new MetaClassMethod.ExceptionTable();
                        et.startPC = ((CodeAttribute) attribute).table[k].startPc.getValue();
                        et.endPC = ((CodeAttribute) attribute).table[k].endPc.getValue();
                        et.handlePC = ((CodeAttribute) attribute).table[k].handlerPc.getValue();
                        et.catchTypeName = cp.getClassName(((CodeAttribute) attribute).table[k].catchType.getValue());
                        table.add(et);

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
                            extension));
        }
    }

    private boolean getFlag(int value, int flag) {
        switch (flag) {
            case MethodAccessProperty.ACC_ABSTRACT:
                return (value & MethodAccessProperty.ACC_ABSTRACT) == 400;
            case MethodAccessProperty.ACC_STATIC:
                return (value & MethodAccessProperty.ACC_STATIC) == 8;
            case MethodAccessProperty.ACC_SYNCHRONIZED:
                return (value & MethodAccessProperty.ACC_SYNCHRONIZED) == 20;
            case MethodAccessProperty.ACC_NATIVE:
                return (value & MethodAccessProperty.ACC_NATIVE) == 100;
            case MethodAccessProperty.ACC_PROTECTED:
                return (value & MethodAccessProperty.ACC_PROTECTED) == 4;
        }
        return false;
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

    public class MethodExtension {
        public ArrayList<Attribute> attrs;
        public boolean isSynchronized;
        public boolean isStatic;
        public boolean isAbstract;
        public boolean isNative;
        public boolean isProtected;
    }
}
