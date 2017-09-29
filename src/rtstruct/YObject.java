package rtstruct;

import rtstruct.meta.MetaClass;

public class YObject{
    private MetaClass metaClassReference;
    private Object[] fields;

    /***************************************************************
     *  create a primitive type YObject. as*() methods stuffing this
     *  object as if assignment operation.
     *
     *  And all deriveFrom(_Primitive x) methods are static methods,
     *  they can create a YObject object representing primitive type
     *
     ***************************************************************/
    public YObject(){
        fields = new Object[1];
        metaClassReference = null;
    }

    /***************************************************************
     *  create a array type YObject. Note that the YObject object of
     *  the array type just store components , the length of the
     *  array is stored at YArray object.
     *
     ***************************************************************/

    public YObject(int dimension) {
        fields = new Object[dimension];
        metaClassReference = null;
    }

    /***************************************************************
     *  create a generic object
     *
     ***************************************************************/
    public YObject(MetaClass metaClass) {
        fields = null;
        metaClassReference = metaClass;
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(int x) {
        return new YObject().asInteger(x);
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(long x) {
        return new YObject().asLong(x);
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(double x) {
        return new YObject().asDouble(x);
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(float x) {
        return new YObject().asFloat(x);
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(String x) {
        return new YObject().asString(x);
    }

    @SuppressWarnings("unused")
    YObject asInteger(int x){
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    YObject asLong(long x){
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    YObject asDouble(double x){
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    YObject asFloat(float x){
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    YObject asString(String x) {
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    public int toInteger(){
        return (int) fields[0];
    }

    @SuppressWarnings("unused")
    public long toLong(){
        return (long) fields[0];
    }

    @SuppressWarnings("unused")
    public double toDouble(){
        return (double) fields[0];
    }

    @SuppressWarnings("unused")
    public float toFloat(){
        return (float) fields[0];
    }

    @SuppressWarnings("unused")
    public short toShort() {
        return (short) fields[0];
    }

    public void setArrayComponent(int index, YObject value) {
        fields[index] = value;
    }

    public YObject getArrayComponent(int index) {
        return (YObject) fields[index];
    }

    public String getClassName() {
        return metaClassReference.qualifiedClassName;
    }
}
