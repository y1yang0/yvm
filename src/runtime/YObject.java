package runtime;

import common.Tuple4;
import common.Tuple6;
import runtime.meta.MetaClass;
import runtime.rtexception.VMExecutionException;
import ycloader.YClassLoader;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.auxil.Peel;

import java.util.Map;

public class YObject{
    private MetaClass metaClassReference;
    private Object[] fields;
    private boolean fieldsInitialized;
    /***************************************************************
     *  create a primitive type YObject. as*() methods stuffing this
     *  object as if assignment operation.
     *
     *  And all deriveFrom(_Primitive x) methods are static methods,
     *  they can create a YObject object representing primitive type
     *
     ***************************************************************/
    @SuppressWarnings("unused")
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

    @SuppressWarnings("unused")
    public YObject(int dimension) {
        fields = new Object[dimension];
        metaClassReference = null;
    }

    /***************************************************************
     *  create a generic object
     *
     ***************************************************************/
    @SuppressWarnings("unused")
    public YObject(MetaClass metaClass) {
        metaClassReference = metaClass;
        fields = null;
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
    public static YObject derivedFrom(char x) {
        return new YObject().asChar(x);
    }

    @SuppressWarnings("unused")
    public static YObject derivedFrom(boolean x) {
        return new YObject().asBoolean(x);
    }

    @SuppressWarnings("unused")
    public MetaClass getMetaClassReference() {
        return metaClassReference;
    }

    @SuppressWarnings("unused")
    public void initiateFields(YClassLoader loader) {
        /***************************************************************
         *  get all fields of this class
         *
         ***************************************************************/
        Map fieldDesc = metaClassReference.fields.getFields();
        int fieldsNum = fieldDesc.size();

        /***************************************************************
         *  create real fields, they will be used to store fields data
         *
         ***************************************************************/
        fields = new Object[fieldsNum];


        /***************************************************************
         *  create a constant class,because in anonymous function, any
         *  reference which referred to outer state required to FINAL
         *
         ***************************************************************/

        class Counter {
            private int i = 0;

            public void inc() {
                i++;
            }

            public int get() {
                return i;
            }

        }
        final Counter counter = new Counter();
        /***************************************************************
         *  traverse all fields and initiate them using default value
         *  according to JVM (R) 8 SPEC
         *
         ***************************************************************/
        fieldDesc.forEach((_Unused, bundle) -> {
            counter.inc();
            //if this field is not loaded into vm, load it right now
            if (!loader
                    .getStartupThread()
                    .runtimeVM()
                    .methodScope()
                    .existClass(Peel.peelFieldDescriptor(((Tuple4) bundle).get2Placeholder().toString()).get(0), loader.getClass())) {
                try {
                    Tuple6 bundle1 = loader.loadClass(Peel.peelFieldDescriptor(((Tuple4) bundle).get2Placeholder().toString()).get(0));
                    MetaClass meta = loader.linkClass(bundle1);
                    loader.getStartupThread().runtimeVM().methodScope().addMetaClass(meta);
                    loader.loadInheritanceChain(meta.superClassName);
                    loader.initializeClass(meta);
                } catch (ClassInitializingException | ClassLinkingException | ClassLoadingException e) {
                    throw new VMExecutionException("can not load class" + Peel.peelFieldDescriptor(Peel.peelFieldDescriptor(((Tuple4) bundle).get2Placeholder().toString()).get(0))
                            + " while executing anewarray opcode");
                }
            }

            //if is a primitive type, assign a default value, or create a new instance
            switch (Peel.peelFieldDescriptor(((Tuple4) bundle).get2Placeholder().toString()).get(0)) {
                case "java/lang/Byte":
                case "java/lang/Short":
                case "java/lang/Long":
                case "java/lang/Integer":
                    fields[counter.get()] = YObject.derivedFrom(0);
                    break;
                case "java/lang/Character":
                    fields[counter.get()] = YObject.derivedFrom('\u0000');
                    break;
                case "java/lang/Double":
                    fields[counter.get()] = YObject.derivedFrom(0.0);
                    break;
                case "java/lang/Float":
                    fields[counter.get()] = YObject.derivedFrom(0.0F);
                    break;
                case "java/lang/Boolean":
                    fields[counter.get()] = YObject.derivedFrom(false);
                    break;
                default:
                    fields[counter.get()] = new YObject(
                            loader.getStartupThread()
                                    .runtimeVM()
                                    .methodScope()
                                    .getMetaClass(
                                            Peel.peelFieldDescriptor(
                                                    ((Tuple4) bundle).get2Placeholder().toString()).get(0),
                                            loader.getClass()));
                    break;
            }

        });
        /***************************************************************
         *  set fieldsInitialized flag to true
         *
         ***************************************************************/
        fieldsInitialized = true;
    }

    @SuppressWarnings("unused")
    public YObject getField(int index) {
        return (YObject) fields[index];
    }

    @SuppressWarnings("unused")
    public void setField(int index, YObject value) {
        fields[index] = value;
    }

    @SuppressWarnings("unused")
    public boolean isInitialized() {
        return fieldsInitialized;
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
    YObject asBoolean(boolean x) {
        fields[0] = x;
        return this;
    }

    @SuppressWarnings("unused")
    YObject asChar(char x) {
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
    public String toString() {
        return (String) fields[0];
    }

    @SuppressWarnings("unused")
    public boolean toBoolean() {
        return (boolean) fields[0];
    }

    @SuppressWarnings("unused")
    public char toChar() {
        return (char) fields[0];
    }

    @SuppressWarnings("unused")
    void setArrayComponent(int index, YObject value) {
        fields[index] = value;
    }

    @SuppressWarnings("unused")
    YObject getArrayComponent(int index) {
        return (YObject) fields[index];
    }

    @SuppressWarnings("unused")
    public String getClassName() {
        return metaClassReference.qualifiedClassName;
    }
}
