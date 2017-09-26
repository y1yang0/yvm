package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import yvm.adt.Tuple4;

import java.util.HashMap;
import java.util.Map;

public class MetaClass {
    public MetaClassField fields;
    private int accessFlag;
    private boolean isClass;
    private String qualifiedClassName;
    private String superClassName;
    private Class classLoader;
    private Map<Integer, Tuple4<String, String, Integer, Attribute[]>> staticVars;
    private Class classReference;
    private MetaClassInterface interfaces;
    private MetaClassConstantPool constantPool;
    private MetaClassMethod methods;


    public MetaClass() {
        staticVars = new HashMap<>();
    }

    public void setAccessFlag(int accessFlag) {
        this.accessFlag = accessFlag;
    }

    public void setSuperClassName(String name) {
        this.superClassName = name;
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

    public Map<Integer, Tuple4<String, String, Integer, Attribute[]>> getStaticVariable() {
        return staticVars;
    }

    public MetaClassMethod getMethods() {
        return methods;
    }

    public void setMethods(MetaClassMethod methods) {
        this.methods = methods;
    }

    public MetaClassConstantPool getConstantPool() {
        return constantPool;
    }

    public void setConstantPool(MetaClassConstantPool constantPool) {
        this.constantPool = constantPool;
    }

    public String getQualifiedClassName() {
        return qualifiedClassName;
    }

    public void setQualifiedClassName(String name) {
        this.qualifiedClassName = name;
    }

    public Class getClassLoader() {
        return classLoader;
    }

    public void setClassLoader(Class classLoaderClass) {
        this.classLoader = classLoaderClass;
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
