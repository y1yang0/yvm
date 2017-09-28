package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import yvm.adt.Tuple4;

import java.util.HashMap;
import java.util.Map;

public class MetaClass {
    public MetaClassField fields;
    public String superClassName;
    public int accessFlag;
    public String qualifiedClassName;
    public Class classLoader;
    public MetaClassInterface interfaces;
    public MetaClassConstantPool constantPool;
    public MetaClassMethod methods;
    private Map<Integer, Tuple4<String, String, Integer, Attribute[]>> staticVars;
    //private boolean isClass;
    //private Class classReference;

    public MetaClass() {
        staticVars = new HashMap<>();
    }


    public void addStaticVariables(int index, Tuple4<String, String, Integer, Attribute[]> staticVar) {
        staticVars.put(index, staticVar);
    }

    public Map<Integer, Tuple4<String, String, Integer, Attribute[]>> getStaticVariable() {
        return staticVars;
    }

    @Override
    public int hashCode() {
        return qualifiedClassName.length();
    }

    @Override
    public boolean equals(Object x) {
        if (x instanceof MetaClass) {
            if (qualifiedClassName.equals(((MetaClass) x).qualifiedClassName)
                    && classLoader == ((MetaClass) x).classLoader) {
                return true;
            }
        }
        return false;
    }
}
