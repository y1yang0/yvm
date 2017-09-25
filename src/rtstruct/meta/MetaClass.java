package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import ycloader.dataobject.ConstantPoolObject;
import yvm.adt.Tuple4;

import java.util.HashMap;
import java.util.Map;

public class MetaClass {
    private int accessFlag;
    private boolean isClass;
    private String qualifiedClassName;
    private String superClassName;
    private Class classLoader;
    private Map<Integer, Tuple4<String, String, Integer, Attribute[]>> staticVars;
    private Class classReference;
    private MetaClassInterface interfaces;
    private ConstantPoolObject constantPool;
    private MetaClassField fields;
    private MetaClassMethod methods;


    public MetaClass() {
        staticVars = new HashMap<>();
    }

    public void setConstantPool(ConstantPoolObject constantPool) {
        this.constantPool = constantPool;
    }

    public void setAccessFlag(int accessFlag) {
        this.accessFlag = accessFlag;
    }

    public void setQualifiedClassName(String name) {
        this.qualifiedClassName = name;
    }

    public void setSuperClassName(String name) {
        this.superClassName = name;
    }

    public void setClassLoader(Class classLoaderClass) {
        this.classLoader = classLoaderClass;
    }

    public void setInterfaces(MetaClassInterface interfaces) {
        this.interfaces = interfaces;
    }

    public void setFields(MetaClassField fields) {
        this.fields = fields;
    }

    public void addStaticVariables(int index, Tuple4<String, String, Integer, Attribute[]> staticVar) {
        staticVars.put(index, staticVar);
    }

    public void setMethods(MetaClassMethod methods) {
        this.methods = methods;
    }

    public Map<Integer, Tuple4<String, String, Integer, Attribute[]>> getStaticVariable() {
        return staticVars;
    }


    public MetaClassMethod getMethods() {
        return methods;
    }

    public ConstantPoolObject getConstantPool() {
        return constantPool;
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
