package rtstruct.meta;

import ycloader.adt.attribute.Attribute;
import ycloader.dataobject.ConstantPoolObject;
import yvm.adt.Tuple4;

import java.util.HashMap;
import java.util.Map;

public class MetaClass {
    private String qualifiedClassName;
    private Class classLoader;
    private String superClassName;
    private MetaClassInterface interfaces;
    private int accessFlag;
    private boolean isClass;
    private ConstantPoolObject constantPool;
    private MetaClassField fields;
    private MetaClassMethod methods;
    private Map<Integer, Tuple4<String, String, Integer, Attribute[]>> staticVars;
    private Class classReference;

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

    public Map<Integer, Tuple4<String, String, Integer, Attribute[]>> getStaticVariable() {
        return staticVars;
    }

    public MetaClassMethod getMethods() {
        return methods;
    }

    public void setMethods(MetaClassMethod methods) {
        this.methods = methods;
    }
}
