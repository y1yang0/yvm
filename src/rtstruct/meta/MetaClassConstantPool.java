package rtstruct.meta;

import rtstruct.YMethodScope;
import rtstruct.YThread;
import ycloader.YClassLoader;
import ycloader.adt.constantpool.*;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.adt.Tuple2;
import yvm.adt.Tuple3;
import yvm.auxil.Predicate;

import java.util.ArrayList;
import java.util.HashMap;

public class MetaClassConstantPool {
    private HashMap<                    //
            Integer,                    //cp index
            String>                     //class name
            classes;                    //

    private HashMap<                    //
            Integer,                    //cp index
            Tuple3<                     //
                    String,             //class name of class method/field name,or interface name of interface method
                    String,             //class method/interface method/field name
                    String>>            //class method/interface method/field descriptor
            refKind;

    private HashMap<
            Integer,                    //cp index
            String>                     //literal string
            strings;                    //

    private HashMap<                    //
            Integer,                    //cp index
            Integer>                    //integer representation
            integers;                   //

    private HashMap<                    //
            Integer,                    //cp index
            Float>                      //float representation
            floats;                     //

    private HashMap<                    //
            Integer,                    //cp index
            Long>                       //long representation
            longs;                      //

    private HashMap<                    //
            Integer,                    //cp index
            Double>                     //double representation
            doubles;                    //

    private HashMap<
            Integer,                    //cp index
            Tuple2<                     //
                    String              //name
                    , String>>           //type
            nameAndTypes;

    private HashMap<                    //
            Integer,                    //cp index
            String>                     //utf8 representation
            utf8s;

    private HashMap<
            Integer,                    //cp index
            Integer>                    //reference index;
            methodHandles;

    private HashMap<
            Integer,                    //cp index
            String>                     //method type name;
            methodTypes;

    private HashMap<
            Integer,                    //cp index
            Tuple3<                     //
                    Integer,            //bootstrap method attribute index
                    String,             //name
                    String>>            //type
            invokeDynamics;

    public MetaClassConstantPool() {
        classes = new HashMap<>();
        refKind = new HashMap<>();
        strings = new HashMap<>();
        integers = new HashMap<>();
        floats = new HashMap<>();
        longs = new HashMap<>();
        doubles = new HashMap<>();
        nameAndTypes = new HashMap<>();
        utf8s = new HashMap<>();
        methodTypes = new HashMap<>();
        methodHandles = new HashMap<>();
        invokeDynamics = new HashMap<>();
    }

    public void resolve(ConstantPoolObject cp) throws ClassLinkingException {
        ArrayList<AbstractConstantPool> acp = cp.getAbstractConstantPool();
        for (AbstractConstantPool x : acp) {
            int index = x.getTableIndex();

            if (x instanceof ConstantClassInfo) {
                String qualifiedClassName = cp.at(((ConstantClassInfo) x).nameIndex.getValue()).toString();

                classes.put(index, qualifiedClassName);
            } else if (x instanceof ConstantFieldRefInfo) {
                int classIndex = ((ConstantFieldRefInfo) x).classIndex.getValue();
                String qualifiedClassName = cp.at(((ConstantClassInfo) cp.at(classIndex)).nameIndex.getValue()).toString();

                int nameAndTypeIndex = ((ConstantFieldRefInfo) x).nameAndTypeIndex.getValue();
                String name = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).nameIndex.getValue()).toString();
                String type = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).descriptorIndex.getValue()).toString();

                refKind.put(index, new Tuple3<>(qualifiedClassName, name, type));
            } else if (x instanceof ConstantMethodRefInfo) {
                int classIndex = ((ConstantMethodRefInfo) x).classIndex.getValue();
                String qualifiedClassName = cp.at(((ConstantClassInfo) cp.at(classIndex)).nameIndex.getValue()).toString();

                int nameAndTypeIndex = ((ConstantMethodRefInfo) x).nameAndTypeIndex.getValue();
                String name = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).nameIndex.getValue()).toString();
                String type = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).descriptorIndex.getValue()).toString();

                refKind.put(index, new Tuple3<>(qualifiedClassName, name, type));
            } else if (x instanceof ConstantInterfaceMethodRefInfo) {
                int classIndex = ((ConstantInterfaceMethodRefInfo) x).classIndex.getValue();
                String qualifiedClassName = cp.at(((ConstantClassInfo) cp.at(classIndex)).nameIndex.getValue()).toString();

                int nameAndTypeIndex = ((ConstantInterfaceMethodRefInfo) x).nameAndTypeIndex.getValue();
                String name = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).nameIndex.getValue()).toString();
                String type = cp.at(((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).descriptorIndex.getValue()).toString();

                refKind.put(index, new Tuple3<>(qualifiedClassName, name, type));
            } else if (x instanceof ConstantStringInfo) {
                String literalString = cp.at(((ConstantStringInfo) x).stringIndex.getValue()).toString();

                strings.put(index, literalString);
            } else if (x instanceof ConstantIntegerInfo) {
                int integer = (int) ((ConstantIntegerInfo) x).bytes.getValue();

                integers.put(index, integer);
            } else if (x instanceof ConstantFloatInfo) {
                int bits = (int) ((ConstantFloatInfo) x).bytes.getValue();
                int s = ((bits >> 31) == 0) ? 1 : -1;
                int e = ((bits >> 23) & 0xff);
                int m = (e == 0) ?
                        (bits & 0x7fffff) << 1 :
                        (bits & 0x7fffff) | 0x800000;
                float floatV = (float) (s * m * Math.pow(2, e - 150));
                floats.put(index, floatV);
            } else if (x instanceof ConstantLongInfo) {
                int high = (int) ((ConstantLongInfo) x).highBytes.getValue();
                int low = (int) ((ConstantLongInfo) x).lowBytes.getValue();
                long longV = ((long) high << 32) + low;
                longs.put(index, longV);
            } else if (x instanceof ConstantDoubleInfo) {
                int high = (int) ((ConstantDoubleInfo) x).highBytes.getValue();
                int low = (int) ((ConstantDoubleInfo) x).lowBytes.getValue();
                long bits = ((long) high << 32) + low;
                int s = ((bits >> 63) == 0) ? 1 : -1;
                int e = (int) ((bits >> 52) & 0x7ffL);
                long m = (e == 0) ?
                        (bits & 0xfffffffffffffL) << 1 :
                        (bits & 0xfffffffffffffL) | 0x10000000000000L;
                double doubleV = s * m * Math.pow(2.0, e - 1075);
                doubles.put(index, doubleV);
            } else if (x instanceof ConstantNameAndTypeInfo) {
                int nameIndex = ((ConstantNameAndTypeInfo) x).nameIndex.getValue();
                int descriptorIndex = ((ConstantNameAndTypeInfo) x).descriptorIndex.getValue();
                String name = cp.at(nameIndex).toString();
                String descriptor = cp.at(descriptorIndex).toString();

                nameAndTypes.put(index, new Tuple2<>(name, descriptor));
            } else if (x instanceof ConstantUtf8Info) {
                utf8s.put(index, x.toString());
            } else if (x instanceof ConstantMethodHandleInfo) {
                int referenceIndex = ((ConstantMethodHandleInfo) x).referenceIndex.getValue();
                methodHandles.put(index, referenceIndex);
            } else if (x instanceof ConstantMethodTypeInfo) {
                int descriptorIndex = ((ConstantMethodTypeInfo) x).descriptorIndex.getValue();
                String descriptor = cp.at(descriptorIndex).toString();

                methodTypes.put(index, descriptor);
            } else if (x instanceof ConstantInvokeDynamicInfo) {
                int bootstrapMethodAttributeIndex = ((ConstantInvokeDynamicInfo) x).bootstrapMethodAttrIndex.getValue();
                int nameAndTypeIndex = ((ConstantInvokeDynamicInfo) x).nameAndTypeIndex.getValue();
                int nameIndex = ((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).nameIndex.getValue();
                int typeIndex = ((ConstantNameAndTypeInfo) cp.at(nameAndTypeIndex)).descriptorIndex.getValue();
                String name = cp.at(nameIndex).toString();
                String type = cp.at(typeIndex).toString();

                invokeDynamics.put(index, new Tuple3<>(bootstrapMethodAttributeIndex, name, type));
            } else {
                throw new ClassLinkingException("unrecognized constant pool type in resolving phase");
            }
        }
    }

    public String findInClasses(int index) {
        return classes.get(index);
    }

    public void debug(String thisClassName) {
        System.out.println("########################" + thisClassName + " Constant Pool#################################");
        System.out.println("#Class#");
        classes.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Reference#");
        refKind.forEach((Index, Bundle) -> {
            System.out.println("#" + Index + "\t" + Bundle.get1Placeholder() + "," + Bundle.get2Placeholder() + "," + Bundle.get3Placeholder() + "#");
        });
        System.out.println("#Strings#");
        strings.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Integer#");
        integers.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Float#");
        floats.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Long#");
        longs.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Double#");
        doubles.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Name and Type#");
        nameAndTypes.forEach((Index, Bundle) -> {
            System.out.println("#" + Index + "\t" + Bundle.get1Placeholder() + "," + Bundle.get2Placeholder() + "#");
        });
        System.out.println("#UTF 8#");
        utf8s.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Method handle#");
        methodHandles.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Method type#");
        methodTypes.forEach((Index, Str) -> {
            System.out.println("#" + Index + "\t" + Str + "#");
        });
        System.out.println("#Invoke dynamic#");
        invokeDynamics.forEach((Index, Bundle) -> {
            System.out.println("#" + Index + "\t" + Bundle.get1Placeholder() + "," + Bundle.get2Placeholder() + "," + Bundle.get3Placeholder() + "#");
        });
        System.out.println("###############################################################################");
    }

    public Integer findInInteger(int index) {
        Integer a = integers.get(index);
        if (!Predicate.isNull(a)) {
            return a;
        }
        return null;
    }

    public Float findInFloat(int index) {
        Float a = floats.get(index);
        if (!Predicate.isNull(a)) {
            return a;
        }
        return null;
    }

    public String findInString(int index) {
        String a = strings.get(index);
        if (!Predicate.isNull(a)) {
            return a;
        }
        return null;
    }

    public Class findInClass(YMethodScope methodScope, YThread thread, YClassLoader loader, int index)
            throws ClassLoadingException, ClassLinkingException, ClassInitializingException {
        String className = classes.get(index);
        if (className != null) {
            if (!methodScope.existClass(className, loader.getClass())) {
                loader.loadInheritanceChain(methodScope, thread, loader, className);
                return methodScope.getMetaClass(className, loader.getClass()).getClass();
            } else {
                return methodScope.getMetaClass(className, loader.getClass()).getClass();
            }
        } else {
            return null;
        }
    }
}
