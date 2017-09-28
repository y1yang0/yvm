package yvm.exec;

import rtstruct.YArray;
import rtstruct.YMethodScope;
import rtstruct.YObject;
import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import rtstruct.meta.MetaClassConstantPool;
import rtstruct.meta.MetaClassMethod;
import rtstruct.rtexception.VMExecutionException;
import rtstruct.ystack.YStack;
import rtstruct.ystack.YStackFrame;
import ycloader.YClassLoader;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import yvm.adt.*;
import yvm.auxil.Continuation;
import yvm.auxil.Peel;
import yvm.auxil.Predicate;

import java.util.ArrayList;

import static java.util.Objects.isNull;
import static yvm.auxil.Predicate.*;

public final class CodeExecutionEngine {
    private YThread thread;
    private boolean ignited;
    private MetaClass metaClassRef;
    private YMethodScope methodScopeRef;
    private YClassLoader classLoader;

    public CodeExecutionEngine() {
        ignited = false;
    }

    public void ignite(MetaClass metaClassInfo, YClassLoader loader) {
        this.classLoader = loader;
        this.metaClassRef = metaClassInfo;
        this.methodScopeRef = loader.getStartupThread().runtimeVM().methodScope();
        this.thread = loader.getStartupThread();
        ignited = true;
    }


    public void executeCLinit() {
        if (!ignited) {
            throw new VMExecutionException("code execution engine is not ready");
        }

        Tuple6<String,                                  //method name
                String,                                 //method descriptor
                u1[],                                   //method codes
                MetaClassMethod.StackRequirement,       //stack requirement for this method
                MetaClassMethod.ExceptionTable[],       //method exception tables,they are differ from checked exception in function signature
                ArrayList<Attribute>>                   ////method related attributes,it would be use for future vm version,there just ignore them
                clinit = metaClassRef.methods.findMethod("<clinit>");

        if (isNull(clinit) || strNotEqual(clinit.get1Placeholder(), "<clinit>")) {
            return;
        }

        int maxLocals = clinit.get4Placeholder().maxLocals;
        int maxStack = clinit.get4Placeholder().maxStack;

        YStackFrame frame = new YStackFrame();
        frame.allocateSize(maxStack, maxLocals);
        thread.runtimeThread().stack().pushFrame(frame);


        try {
            Opcode op = new Opcode(clinit.get3Placeholder());
            op.codes2Opcodes();
            op.debug(metaClassRef.qualifiedClassName + " clinit");

        } catch (ClassInitializingException ignored) {
            throw new VMExecutionException("failed to convert binary code to opcodes in executing <clinit> method");
        }
        //codeExecution(op);
        thread.runtimeThread().stack().popFrame();
    }

    @SuppressWarnings({"unchecked","unused"})
    private void codeExecution(Opcode op){
        YStack stack = thread.runtimeThread().stack();
        class ConvenientDelegate {
            private Object peek() {
                return stack.currentFrame().peekOperand();
            }
            private Object pop() {
                return stack.currentFrame().popOperand();
            }
            private void push(Object object) {
                stack.currentFrame().pushOperand(object);
            }
            private void setLocalVar(int index,Object value){
                stack.currentFrame().setLocalVariable(index,value);
            }
            private Object getLocalVar(int index){
                return stack.currentFrame().getLocalVariable(index);
            }
            private int popInt(){return (int) stack.currentFrame().popOperand();}
            private double popDouble(){return (double) stack.currentFrame().popOperand();}
            private long popLong(){return (long) stack.currentFrame().popOperand();}
            private float popFloat(){return (float) stack.currentFrame().popOperand();}
        }
        ConvenientDelegate dg = new ConvenientDelegate();//use a convenient delegate class to wrap stack operations

        ArrayList<                                      //get opcode list from argument "op" of Opcode
                Tuple3<                                 //
                        Integer,                        //program counter
                        Integer,                        //opcode numeric value
                        Operand>>                       //operand of this opcode
                opcodes = op.getOpcodes();              //


        //opcode execution

        for (int i = 0; i < opcodes.size(); i++) {
            Tuple3 cd = opcodes.get(i);
            int programCount = (Integer) cd.get1Placeholder();
            thread.runtimeThread().pc(programCount);
            switch ((Integer) cd.get2Placeholder()) {

                //Load reference from array
                case Mnemonic.aaload: {
                    int index = dg.popInt();
                    YArray arrayRef = (YArray) dg.pop();

                    Continuation.ifNullThrowNullptrException(arrayRef);

                    if (!inRange(arrayRef, index)) {
                        throw new ArrayIndexOutOfBoundsException("array index " + index + " out of bounds");
                    }
                    dg.push(arrayRef.get(index));
                }
                break;

                //Store into reference array
                case Mnemonic.aastore: {
                    Object value = dg.pop();
                    int index = dg.popInt();
                    YArray arrayRef = (YArray) dg.pop();

                    if (isClass(value.getClass())) {
                        if (!isSameClass(arrayRef.getClass().getComponentType(), value.getClass())
                                && value.getClass().isInstance(arrayRef.getClass().getComponentType())) {
                            throw new ArrayStoreException("incorrect value type to be stored into an array");
                        }
                    }
                    if (isInterface(value.getClass())) {
                        if (isClass(arrayRef.getClass().getComponentType())) {
                            if (!isSameClass(arrayRef.getClass().getComponentType(), Object.class)) {
                                throw new ArrayStoreException("incorrect value type to be stored into an array");
                            }
                        } else if (isInterface(arrayRef.getClass().getComponentType())) {
                            if (!value.getClass().isInstance(arrayRef.getClass().getComponentType())) {
                                throw new ArrayStoreException("incorrect value type to be stored into an array");
                            }
                        }
                    } else if (isArray(value.getClass())) {
                        if (isClass(arrayRef.getClass().getComponentType())) {
                            if (!isSameClass(arrayRef.getClass().getComponentType(), Object.class)) {
                                throw new ArrayStoreException("incorrect value type to be stored into an array");
                            }
                        } else if (isArray(arrayRef.getClass().getComponentType())) {
                            if (!isSameClass(
                                    arrayRef.getClass().getComponentType().getComponentType(),
                                    value.getClass().getComponentType()
                            )) {
                                throw new ArrayStoreException("incorrect value type to be stored into an array");
                            }
                        } else if (isInterface(arrayRef.getClass().getComponentType())) {
                            if (!value.getClass().isInstance(arrayRef.getClass().getComponentType())) {
                                throw new ArrayStoreException("incorrect value type to be stored into an array");
                            }
                        }
                    }


                    arrayRef.set(index, value);
                }
                break;

                //Push null
                case Mnemonic.aconst_null: {
                    dg.push(null);
                }
                break;

                //Load reference from local variable
                case Mnemonic.aload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object objectRef = dg.getLocalVar(index);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 0
                case Mnemonic.aload_0: {
                    Object objectRef = dg.getLocalVar(0);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 1
                case Mnemonic.aload_1: {
                    Object objectRef = dg.getLocalVar(1);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 2
                case Mnemonic.aload_2: {
                    Object objectRef = dg.getLocalVar(2);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 3
                case Mnemonic.aload_3: {
                    Object objectRef = dg.getLocalVar(3);
                    dg.push(objectRef);
                }
                break;

                //Create new array of reference
                case Mnemonic.anewarray: {
                    //The count represents the number of components of the array to
                    //be created.
                    int count = dg.popInt();
                    byte indexByte1 = (byte) ((Operand) cd.get3Placeholder()).get0();
                    byte indexByte2 = (byte) ((Operand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;
                    String res = metaClassRef.constantPool.findInClasses(index);

                    if (count < 0) {
                        throw new NegativeArraySizeException("array size required a positive integer");
                    }
                    if (!isNull(res)) {
                        if (methodScopeRef.existClass(res, classLoader.getClass())) {
                            MetaClass meta = methodScopeRef.getMetaClass(res, metaClassRef.classLoader);
                            YObject object = new YObject(meta);
                            object.init();

                            YArray array = new YArray(count);
                            array.init();

                            dg.push(array);
                        } else {
                            String arrayComponentType = Peel.getArrayComponent(res);
                            int arrayDimension = Peel.getArrayDimension(res);
                            if (methodScopeRef.existClass(arrayComponentType, classLoader.getClass())) {
                                MetaClass meta = methodScopeRef.getMetaClass(res, metaClassRef.classLoader);
                                YObject object = new YObject(meta);
                                object.init();

                                YArray inner = new YArray(arrayDimension);
                                inner.init();

                                YArray outer = new YArray(count);
                                outer.init();

                                dg.push(outer);
                            }
                        }
                    }
                }
                break;

                case Mnemonic.areturn: {
                    //todo:areturn
                }
                break;

                //Get length of array
                case Mnemonic.arraylength: {
                    YArray arrayRef = (YArray) dg.pop();
                    Continuation.ifNullThrowNullptrException(arrayRef);
                    dg.push(arrayRef.getDimension());
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = dg.pop();

                    dg.setLocalVar(index, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_0: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = dg.pop();

                    dg.setLocalVar(0, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_1: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = dg.pop();

                    dg.setLocalVar(1, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_2: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = dg.pop();

                    dg.setLocalVar(2, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_3: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = dg.pop();

                    dg.setLocalVar(3, top);
                }
                break;

                case Mnemonic.athrow: {
                    //todo:athrow
                }
                break;

                //Load byte or boolean from array
                case Mnemonic.baload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();

                    Continuation.ifNullThrowNullptrException(array);

                    if (index > array.getDimension()) {
                        throw new ArrayIndexOutOfBoundsException("array index out of bounds");
                    }
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.bastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.bipush: {
                    dg.push(((Operand) cd.get3Placeholder()).get0());
                }
                break;

                case Mnemonic.caload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.castore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.checkcast: {
                    //todo:checkcast
                }
                break;

                case Mnemonic.d2f: {
                    double value = dg.popDouble();
                    dg.push((float) value);
                }
                break;

                case Mnemonic.d2i: {
                    double value = dg.popDouble();
                    dg.push((int) value);
                }
                break;

                case Mnemonic.d2l: {
                    double value = dg.popDouble();
                    dg.push((long) value);
                }
                break;

                case Mnemonic.dadd: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(value1 + value2);
                }
                break;

                case Mnemonic.daload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.dastore: {
                    double value = dg.popDouble();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.dcmpg:
                case Mnemonic.dcmpl: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    float value1$ = (float) value1;
                    float value2$ = (float) value2;
                    if (value1$ > value2$) {
                        dg.push(0);
                    } else if (value1$ < value2$) {
                        dg.push(-1);
                    } else if ((Math.abs(value1$ - value2$) > 0)) {
                        dg.push(0);
                    }
                }
                break;

                case Mnemonic.dconst_0: {
                    dg.push(0.0);
                }
                break;

                case Mnemonic.dconst_1: {
                    dg.push(1.0);
                }
                break;

                case Mnemonic.ddiv: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(value1 / value2);
                }
                break;

                case Mnemonic.dload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    double value = (double) dg.getLocalVar(index);
                    dg.push(value);
                }
                break;

                case Mnemonic.dload_0: {
                    double value = (double) dg.getLocalVar(0);
                    dg.push(value);
                }
                break;

                case Mnemonic.dload_1: {
                    double value = (double) dg.getLocalVar(1);
                    dg.push(value);
                }
                break;

                case Mnemonic.dload_2: {
                    double value = (double) dg.getLocalVar(2);
                    dg.push(value);
                }
                break;

                case Mnemonic.dload_3: {
                    double value = (double) dg.getLocalVar(3);
                    dg.push(value);
                }
                break;

                case Mnemonic.dmul: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(value1 * value2);
                }

                case Mnemonic.dneg:{
                    double value = dg.popDouble();
                    dg.push(-value);
                }
                break;

                case Mnemonic.drem:{
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(value1 % value2);
                }
                break;

                case Mnemonic.dreturn:{
                    //todo:dreturn
                }
                break;

                case Mnemonic.dstore:{
                    double value = dg.popDouble();
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    dg.setLocalVar(index,value);
                }
                break;

                case Mnemonic.dstore_0:{
                    double value = dg.popDouble();
                    dg.setLocalVar(0,value);
                }
                break;

                case Mnemonic.dstore_1:{
                    double value = dg.popDouble();
                    dg.setLocalVar(1,value);
                }
                break;

                case Mnemonic.dstore_2:{
                    double value = dg.popDouble();
                    dg.setLocalVar(2,value);
                }
                break;

                case Mnemonic.dstore_3:{
                    double value = dg.popDouble();
                    dg.setLocalVar(3,value);
                }
                break;

                case Mnemonic.dsub:{
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(value1 - value2);
                }
                break;

                case Mnemonic.dup:{
                    Object value = dg.pop();
                    dg.push(value);
                    dg.push(value);
                }
                break;

                case Mnemonic.dup_x1:{
                    Object value1 = dg.pop();
                    Object value2 = dg.pop();
                    dg.push(value1);
                    dg.push(value2);
                    dg.push(value1);
                }
                break;

                case Mnemonic.dup_x2:{
                    Object value1 = dg.pop();
                    Object value2 = dg.pop();
                    if (Predicate.isCategory2ComputationalType(value2)) {
                        //category 2 computational type
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        //category 1 computational type
                        Object value3 = dg.pop();
                        dg.push(value1);
                        dg.push(value3);
                        dg.push(value2);
                        dg.push(value1);
                    }
                }
                break;

                case Mnemonic.dup2: {
                    Object value =  dg.pop();
                    if (Predicate.isCategory2ComputationalType(value)) {
                        //category 2 computational type
                        dg.push(value);
                        dg.push(value);
                    }else{
                        //category 1 computational type
                        Object value2 =  dg.pop();
                        dg.push(value2);
                        dg.push(value);
                        dg.push(value2);
                        dg.push(value);
                    }
                }
                break;

                case Mnemonic.dup2_x1:{
                    Object value1=  dg.pop();
                    if (Predicate.isCategory2ComputationalType(value1)) {
                        //category 2 computational type
                        Object value2 = dg.pop();
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        //category 1 computational type
                        Object value2 =  dg.pop();
                        Object value3 =  dg.pop();
                        dg.push(value2);
                        dg.push(value1);
                        dg.push(value3);
                        dg.push(value2);
                        dg.push(value1);
                    }
                }
                break;

                case Mnemonic.dup2_x2:{
                    Object value1=  dg.pop();
                    Object value2=  dg.pop();
                    //Form 4
                    if (Predicate.isCategory2ComputationalType(value1) &&
                            Predicate.isCategory2ComputationalType(value2)) {
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        Object value3 = dg.pop();
                        //Form 3
                        if (Predicate.isCategory1ComputationalType(value1) &&
                                Predicate.isCategory1ComputationalType(value2) &&
                                Predicate.isCategory2ComputationalType(value3)) {
                            dg.push(value2);
                            dg.push(value1);
                            dg.push(value3);
                            dg.push(value2);
                            dg.push(value1);
                        }
                        //Form 2
                        else if (Predicate.isCategory2ComputationalType(value1) &&
                                Predicate.isCategory1ComputationalType(value2) &&
                                Predicate.isCategory1ComputationalType(value3)) {
                            dg.push(value1);
                            dg.push(value3);
                            dg.push(value2);
                            dg.push(value1);
                        }
                        else{
                            Object value4 = dg.pop();
                            //Form 1
                            if (Predicate.isCategory2ComputationalType(value1) &&
                                    Predicate.isCategory2ComputationalType(value2) &&
                                    Predicate.isCategory2ComputationalType(value3) &&
                                    Predicate.isCategory2ComputationalType(value4)) {
                                dg.push(value2);
                                dg.push(value1);
                                dg.push(value4);
                                dg.push(value3);
                                dg.push(value2);
                                dg.push(value1);
                            }
                        }
                    }
                }
                break;

                case Mnemonic.f2d:{
                    float value = dg.popFloat();
                    dg.push((double)value);
                }
                break;

                case Mnemonic.f2i:{
                    float value = dg.popFloat();
                    dg.push((int)value);
                }
                break;

                case Mnemonic.f2l:{
                    float value = dg.popFloat();
                    dg.push((long)value);
                }
                break;

                case Mnemonic.fadd:{
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(value1 + value2);
                }
                break;

                case Mnemonic.faload:{
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.fastore:{
                    float  value = (float)dg.pop();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    array.set(index,value);
                }
                break;

                case Mnemonic.fcmpg:
                case Mnemonic.fcmpl:{
                    float value2 = dg.popFloat();
                    float value1  = dg.popFloat();
                    if(value1 > value2){
                        dg.push(1);
                    }else if(value1 < value2){
                        dg.push(-1);
                    }else if(Math.abs(value1 - value2) > 0){
                        dg.push(0);
                    }
                }
                break;

                case Mnemonic.fconst_0:{
                    dg.push(0.0);
                }
                break;

                case Mnemonic.fconst_1:{
                    dg.push(1.0);
                }
                break;

                case Mnemonic.fconst_2:{
                    dg.push(2.0);
                }
                break;

                case Mnemonic.fdiv:{
                    float value2 = dg.popFloat();
                    float value1  = dg.popFloat();
                    dg.push(value1/value2);
                }
                break;

                case Mnemonic.fload:{
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    dg.push(dg.getLocalVar(index));
                }
                break;

                case Mnemonic.fload_0:{
                    dg.push(dg.getLocalVar(0));
                }
                break;

                case Mnemonic.fload_1:{
                    dg.push(dg.getLocalVar(1));
                }
                break;

                case Mnemonic.fload_2:{
                    dg.push(dg.getLocalVar(2));
                }
                break;

                case Mnemonic.fload_3:{
                    dg.push(dg.getLocalVar(3));
                }
                break;

                case Mnemonic.fmul:{
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(value1*value2);
                }
                break;

                case Mnemonic.fneg:{
                    float value = dg.popFloat();
                    dg.push(-value);
                }
                break;

                case Mnemonic.frem:{
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(value1-(value1/value2));
                }
                break;

                case Mnemonic.freturn:{
                    //todo:freturn
                }
                break;

                case Mnemonic.fstore:{
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    float value = dg.popFloat();
                    dg.setLocalVar(index, value);
                }
                break;

                case Mnemonic.fstore_0: {
                    float value = dg.popFloat();
                    dg.setLocalVar(0, value);
                }
                break;

                case Mnemonic.fstore_1: {
                    float value = dg.popFloat();
                    dg.setLocalVar(1, value);
                }
                break;

                case Mnemonic.fstore_2: {
                    float value = dg.popFloat();
                    dg.setLocalVar(2, value);
                }
                break;

                case Mnemonic.fstore_3: {
                    float value = dg.popFloat();
                    dg.setLocalVar(3, value);
                }
                break;

                case Mnemonic.fsub: {
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(value1 - value2);
                }
                break;

                case Mnemonic.getfield: {
                    //todo:getfield
                }
                break;

                case Mnemonic.getstatic: {
                    //todo:getstatic
                }
                break;

                case Mnemonic.goto$: {
                    int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                    int branchOffset = (branchByte1 << 8) | branchByte2;
                    Tuple3 newOp = opcodes.get(branchOffset);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = currentI;
                }
                break;

                case Mnemonic.goto_w: {
                    /*
                    Although the goto_w instruction takes a 4-byte branch offset, other
                    factors limit the size of a method to 65535 bytes . This limit may
                    be raised in a future release of the Java Virtual Machine.
                     */
                    int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                    int branchByte3 = (int) ((Operand) cd.get3Placeholder()).get2();
                    int branchByte4 = (int) ((Operand) cd.get3Placeholder()).get3();
                    int branchOffset = (branchByte1 << 24) | (branchByte2 << 16)
                            | (branchByte3 << 8) | branchByte4;
                    Tuple3 newOp = opcodes.get(branchOffset);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = currentI;
                }
                break;

                case Mnemonic.i2b: {
                    int value = dg.popInt();
                    byte value$ = (byte) value;
                    dg.push((int) value$);
                }
                break;

                case Mnemonic.i2c: {
                    int value = dg.popInt();
                    char value$ = (char) value;
                    dg.push((int) value$);
                }
                break;

                case Mnemonic.i2d: {
                    int value = dg.popInt();
                    double value$ = (double) value;
                    dg.push(value$);
                }
                break;

                case Mnemonic.i2f: {
                    int value = dg.popInt();
                    float value$ = (float) value;
                    dg.push(value$);
                }
                break;

                case Mnemonic.i2l: {
                    int value = dg.popInt();
                    long value$ = (long) value;
                    dg.push(value$);
                }
                break;

                case Mnemonic.i2s: {
                    int value = dg.popInt();
                    short value$ = (short) value;
                    dg.push((int) value$);
                }
                break;

                case Mnemonic.iadd: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 + value2);
                }
                break;

                case Mnemonic.iaload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.iand: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 & value2);
                }
                break;

                case Mnemonic.iastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.iconst_m1: {
                    dg.push(-1);
                }
                break;

                case Mnemonic.iconst_0: {
                    dg.push(0);
                }
                break;

                case Mnemonic.iconst_1: {
                    dg.push(1);
                }
                break;

                case Mnemonic.iconst_2: {
                    dg.push(2);
                }
                break;

                case Mnemonic.iconst_3: {
                    dg.push(3);
                }
                break;

                case Mnemonic.iconst_4: {
                    dg.push(4);
                }
                break;

                case Mnemonic.iconst_5: {
                    dg.push(5);
                }
                break;

                case Mnemonic.idiv: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }
                    dg.push(value1 / value2);
                }
                break;

                case Mnemonic.if_acmpeq: {
                    YObject value2 = (YObject) dg.pop();
                    YObject value1 = (YObject) dg.pop();
                    if (value1 == value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_acmpne: {
                    YObject value2 = (YObject) dg.pop();
                    YObject value1 = (YObject) dg.pop();
                    if (value1 != value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpeq: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 == value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpne: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 != value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmplt: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 < value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpge: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 >= value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpgt: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 > value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.if_icmple: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 <= value2) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifeq: {
                    int value = dg.popInt();
                    if (value == 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifne: {
                    int value = dg.popInt();
                    if (value != 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.iflt: {
                    int value = dg.popInt();
                    if (value < 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifge: {
                    int value = dg.popInt();
                    if (value >= 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifgt: {
                    int value = dg.popInt();
                    if (value > 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifle: {
                    int value = dg.popInt();
                    if (value <= 0) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifnonnull: {
                    YObject value = (YObject) dg.pop();
                    if (value != null) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.ifnull: {
                    YObject value = (YObject) dg.pop();
                    if (value == null) {
                        int branchByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                        int branchByte2 = (int) ((Operand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = currentI;
                    }
                }
                break;

                case Mnemonic.iinc: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    int const$ = (int) ((Operand) cd.get3Placeholder()).get1();
                    dg.setLocalVar(index, (int) dg.getLocalVar(index) + const$);
                }
                break;

                case Mnemonic.iload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    int value = (int) dg.getLocalVar(index);
                    dg.push(value);
                }
                break;

                case Mnemonic.iload_0: {
                    int value = (int) dg.getLocalVar(0);
                    dg.push(value);
                }
                break;

                case Mnemonic.iload_1: {
                    int value = (int) dg.getLocalVar(1);
                    dg.push(value);
                }
                break;

                case Mnemonic.iload_2: {
                    int value = (int) dg.getLocalVar(2);
                    dg.push(value);
                }
                break;

                case Mnemonic.iload_3: {
                    int value = (int) dg.getLocalVar(3);
                    dg.push(value);
                }
                break;

                case Mnemonic.imul: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 * value2);
                }
                break;

                case Mnemonic.ineg: {
                    int value = dg.popInt();
                    dg.push((~value) + 1);
                }
                break;

                case Mnemonic.instanceof$: {
                    //todo:instanceof
                }
                break;

                case Mnemonic.invokedynamic: {
                    //todo:invokedymaic
                }
                break;

                case Mnemonic.invokeinterface: {
                    //todo:invokeinterface
                }
                break;

                case Mnemonic.invokespecial: {
                    //todo:invokespecial
                }
                break;

                case Mnemonic.invokestatic: {
                    //todo:invokestatic
                }
                break;

                case Mnemonic.invokevirtual: {
                    //todo:invokevirtual
                }
                break;

                case Mnemonic.ior: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 | value2);
                }
                break;

                case Mnemonic.irem: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value2 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }
                    dg.push(value1 - (value1 / value2));
                }
                break;

                case Mnemonic.ireturn: {
                    //todo:ireturn
                }
                break;

                case Mnemonic.ishl: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 << (value2 & 0x1F));
                }
                break;

                case Mnemonic.ishr: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 >> (value2 & 0x1F));
                }
                break;

                case Mnemonic.istore: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    int value = dg.popInt();
                    dg.setLocalVar(index, value);
                }
                break;

                case Mnemonic.istore_0: {
                    int value = dg.popInt();
                    dg.setLocalVar(0, value);
                }
                break;

                case Mnemonic.istore_1: {
                    int value = dg.popInt();
                    dg.setLocalVar(1, value);
                }
                break;

                case Mnemonic.istore_2: {
                    int value = dg.popInt();
                    dg.setLocalVar(2, value);
                }
                break;

                case Mnemonic.istore_3: {
                    int value = dg.popInt();
                    dg.setLocalVar(3, value);
                }
                break;

                case Mnemonic.isub: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 - value2);
                }
                break;

                case Mnemonic.iushr: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 > 0) {
                        dg.push(value1 >> (value2 & 0x1F));
                    } else if (value1 < 0) {
                        dg.push(value1 >> (value2 & 0x1F) + (1 << ~(value2 & 0x1F)));
                    }
                }
                break;

                case Mnemonic.ixor: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(value1 ^ value2);

                }
                break;

                case Mnemonic.jsr:
                case Mnemonic.jsr_w: {
                    throw new VMExecutionException("unsupport the JSR opcode, you may change a posterior compiler " +
                            "version of Java SE 6 ");
                }

                case Mnemonic.l2d: {
                    long value = dg.popLong();
                    dg.push((double) value);
                }
                break;

                case Mnemonic.l2f: {
                    long value = dg.popLong();
                    dg.push((float) value);
                }
                break;

                case Mnemonic.l2i: {
                    long value = dg.popLong();
                    dg.push((int) value);
                }
                break;

                case Mnemonic.ladd: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 + value2);
                }
                break;

                case Mnemonic.laload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.land: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 & value2);
                }
                break;

                case Mnemonic.lastore: {
                    long value = dg.popLong();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();
                    dg.setLocalVar(index, array.get(index));
                }
                break;

                case Mnemonic.lcmp: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    if (value1 > value2) {
                        dg.push(1);
                    } else if (value1 == value2) {
                        dg.push(0);
                    } else if (value1 < value2) {
                        dg.push(-1);
                    }
                }
                break;

                case Mnemonic.lconst_0: {
                    dg.push(0L);
                }
                break;

                case Mnemonic.lconst_1: {
                    dg.push(1L);
                }
                break;

                //Push item from run-time constant pool
                case Mnemonic.ldc: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInFloat(index))) {
                        dg.push(poolRef.findInFloat(index));
                    } else if (!Predicate.isNull(poolRef.findInInteger(index))) {
                        dg.push(poolRef.findInInteger(index));
                    } else if (!Predicate.isNull(poolRef.findInString(index))) {
                        dg.push(poolRef.findInString(index));
                    } else if (!Predicate.isNull(poolRef.findInClass(index))) {
                        Class c = methodScopeRef.getMetaClass(poolRef.findInClass(index), classLoader.getClass()).classLoader;
                        dg.push(c);
                    } else {
                        //todo:if not find class then load it
                        //todo: support methodtype and methodhandle
                    }
                }
                break;

                case Mnemonic.ldc_w: {
                    int indexByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int indexByte2 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int index = (indexByte1 << 8) | indexByte2;

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInFloat(index))) {
                        dg.push(poolRef.findInFloat(index));
                    } else if (!Predicate.isNull(poolRef.findInInteger(index))) {
                        dg.push(poolRef.findInInteger(index));
                    } else if (!Predicate.isNull(poolRef.findInString(index))) {
                        dg.push(poolRef.findInString(index));
                    } else if (!Predicate.isNull(poolRef.findInClass(index))) {
                        Class c = methodScopeRef.getMetaClass(poolRef.findInClass(index), classLoader.getClass()).classLoader;
                        dg.push(c);
                    } else {
                        //todo:if not find class then load it
                        //todo: support methodtype and methodhandle
                    }
                }
                break;

                case Mnemonic.ldc2_w: {
                    int indexByte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int indexByte2 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int index = (indexByte1 << 8) | indexByte2;

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInLong(index))) {
                        dg.push(poolRef.findInLong(index));
                    } else if (!Predicate.isNull(poolRef.findInDouble(index))) {
                        dg.push(poolRef.findInDouble(index));
                    } else {
                        throw new VMExecutionException("ldc_2 has a invalid constant pool entry");
                    }
                }
                break;

                case Mnemonic.ldiv: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 / value2);
                }
                break;

                case Mnemonic.lload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    dg.push(dg.getLocalVar(index));
                }
                break;

                case Mnemonic.lload_0: {
                    dg.push(dg.getLocalVar(0));
                }
                break;

                case Mnemonic.lload_1: {
                    dg.push(dg.getLocalVar(1));
                }
                break;

                case Mnemonic.lload_2: {
                    dg.push(dg.getLocalVar(2));
                }
                break;

                case Mnemonic.lload_3: {
                    dg.push(dg.getLocalVar(3));
                }
                break;

                case Mnemonic.lmul: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 * value2);
                }
                break;

                case Mnemonic.lneg: {
                    long value = dg.popLong();
                    dg.push(-value);
                }
                break;

                case Mnemonic.lookupswitch: {
                    //todo:lookupswitch
                }
                break;

                case Mnemonic.lor: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 | value2);
                }
                break;

                case Mnemonic.lrem: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    if (value1 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }
                    dg.push(value1 - (value1 / value2) * value2);
                }
                break;

                case Mnemonic.lreturn: {
                    //todo:lreturn
                }
                break;

                case Mnemonic.lshl: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 << (value2 & 0x3F));
                }
                break;

                case Mnemonic.lshr: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 >> (value2 & 0x3F));
                }
                break;

                case Mnemonic.lstore: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    long value = dg.popLong();
                    dg.setLocalVar(index, value);
                }
                break;

                case Mnemonic.lstore_0: {
                    long value = dg.popLong();
                    dg.setLocalVar(0, value);
                }
                break;

                case Mnemonic.lstore_1: {
                    long value = dg.popLong();
                    dg.setLocalVar(1, value);
                }
                break;

                case Mnemonic.lstore_2: {
                    long value = dg.popLong();
                    dg.setLocalVar(2, value);
                }
                break;

                case Mnemonic.lstore_3: {
                    long value = dg.popLong();
                    dg.setLocalVar(3, value);
                }
                break;

                case Mnemonic.lsub: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 - value2);
                }
                break;

                case Mnemonic.lushr: {
                    int value2 = dg.popInt();
                    long value1 = dg.popLong();
                    dg.push((value1 >> (value2 & 0x3F)) + (2L << ~(value2 & 0x3F)));
                }
                break;

                case Mnemonic.lxor: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(value1 ^ value2);
                }
                break;

                case Mnemonic.monitorenter: {
                    //todo:monitorenter
                }
                break;

                case Mnemonic.monitorexit: {
                    //todo:monitorexit
                }
                break;

                case Mnemonic.multianewarray: {
                    //todo:multianewarray
                }
                break;

                case Mnemonic.new$: {
                    //todo:new
                }
                break;

                case Mnemonic.newarray: {
                    //todo:newarray
                }
                break;

                case Mnemonic.nop: {
                    //////////////
                    //DO NOTHING//
                    //////////////
                }
                break;

                case Mnemonic.pop: {
                    dg.pop();
                }
                break;

                case Mnemonic.pop2: {
                    Object value = dg.peek();
                    if (Predicate.isCategory2ComputationalType(value)) {
                        dg.pop();
                    } else {
                        Object value1 = dg.pop();
                        Object value2 = dg.peek();
                        if (Predicate.isCategory1ComputationalType(value2) &&
                                Predicate.isCategory1ComputationalType(value1)) {
                            dg.pop();
                        }
                    }
                }
                break;

                case Mnemonic.putfield: {
                    //todo:putfield
                }
                break;

                case Mnemonic.putstatic: {
                    //todo:putstatic
                }
                break;

                case Mnemonic.ret: {
                    //todo:ret
                }
                break;

                case Mnemonic.return$: {
                    //todo:return
                }
                break;

                case Mnemonic.saload: {
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();

                    Continuation.ifNullThrowNullptrException(array);

                    short v = (short) array.get(index);
                    dg.push(v);
                }
                break;

                case Mnemonic.sastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = (YArray) dg.pop();

                    Continuation.ifNullThrowNullptrException(array);

                    array.set(index, (short) value);
                }
                break;

                case Mnemonic.sipush: {
                    int byte1 = (int) ((Operand) cd.get3Placeholder()).get0();
                    int byte2 = (int) ((Operand) cd.get3Placeholder()).get0();
                    short byte$ = (short) ((byte1 <<
                            8) | byte2);
                    dg.push((int) byte$);
                }
                break;

                case Mnemonic.swap: {
                    Object value1 = dg.pop();
                    Object value2 = dg.pop();
                    if (Predicate.isCategory1ComputationalType(value1) &&
                            Predicate.isCategory1ComputationalType(value2)) {
                        Object temp = value1;
                        value1 = value2;
                        value2 = temp;
                        dg.push(value1);
                        dg.push(value2);
                    } else {
                        throw new VMExecutionException("The Java Virtual Machine does not provide an instruction\n" +
                                "implementing a swap on operands of category 2 computational\n" +
                                "types.");
                    }
                }
                break;

                case Mnemonic.tableswitch: {
                    //todo:tableswitch
                }
                break;

                case Mnemonic.wide: {
                    //todo:wide;
                }
                break;

                case Mnemonic.breakpoint: {
                    throw new VMExecutionException("<breakpoint> opcode was not mentioned on Jvm(R) 8 Spec");
                }

                case Mnemonic.impdep1: {
                    throw new VMExecutionException("<impdep1> opcode was not mentioned on Jvm(R) 8 Spec");
                }

                case Mnemonic.impdep2: {
                    throw new VMExecutionException("<impdep2> opcode was not mentioned on Jvm(R) 8 Spec");
                }

                default:
                    throw new VMExecutionException("unknown opcode encountered in execution sequence");
            }
        }
    }
}
