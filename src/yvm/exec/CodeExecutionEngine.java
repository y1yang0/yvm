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
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.adt.*;
import yvm.auxil.Continuation;
import yvm.auxil.Peel;
import yvm.auxil.Predicate;
import yvm.constant.NewArrayType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public final class CodeExecutionEngine {
    @ValueRequired
    private YThread thread;
    @ValueRequired
    private MetaClass metaClassRef;
    @ValueRequired
    private YMethodScope methodScopeRef;
    @ValueRequired
    private YClassLoader classLoader;

    private boolean ignited;
    private Lock methodLock;

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

    @SuppressWarnings("unchecked")
    public void executeMethod(String methodName) {
        if (!ignited) {
            throw new VMExecutionException("code execution engine is not ready");
        }

        Tuple6<String,                                                   //method name
                String,                                                  //method descriptor
                u1[],                                                    //method codes
                MetaClassMethod.StackRequirement,                        //stack requirement for this method
                ArrayList<MetaClassMethod.ExceptionTable>,               //method exception tables,they are differ from checked exception in function signature
                MetaClassMethod.MethodExtension>                         //method related attributes,it would be use for future vm version,there just ignore them
                clinit = metaClassRef.methods.findMethod(methodName);

        if (Predicate.isNull(clinit) || Predicate.strNotEqual(clinit.get1Placeholder(), methodName)) {
            throw new VMExecutionException("method not found");
        }

        int maxLocals = clinit.get4Placeholder().maxLocals;
        int maxStack = clinit.get4Placeholder().maxStack;
        boolean isSynchronized = clinit.get6Placeholder().isSynchronized;
        ArrayList<MetaClassMethod.ExceptionTable> exceptionTable = clinit.get5Placeholder();



        try {
            ValueChecker.safeCheck(this.getClass(), this);

            allocateStackFrame(maxLocals, maxStack);
            Opcode op = new Opcode(clinit.get3Placeholder());
            op.codes2Opcodes();
            op.debug(metaClassRef.qualifiedClassName + " clinit");
            //codeExecution(op,exceptionTable,isSynchronized);

        } catch (ClassInitializingException ignored) {
            throw new VMExecutionException("failed to convert binary code to opcodes in executing <clinit> method");
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }

    }

    @SuppressWarnings({"unchecked","unused"})
    private void codeExecution(Opcode op, ArrayList<MetaClassMethod.ExceptionTable> exceptionTable, boolean isSynchronized) {
        /***************************************************************
         *  get current thread stack reference, and create a convenient
         *  operator class <ConvenientDelegate> to execute push/pop of
         *  stack operations
         *
         ***************************************************************/
        YStack stack = thread.runtimeThread().stack();
        class ConvenientDelegate {
            private void clear() {
                stack.currentFrame().clearOperand();
            }
            private YObject peek() {
                return  stack.currentFrame().peekOperand();
            }
            private YObject pop() {
                return  stack.currentFrame().popOperand();
            }
            private void push(YObject object) {
                stack.currentFrame().pushOperand(object);
            }
            private void setLocalVar(int index,YObject value){
                stack.currentFrame().setLocalVariable(index,value);
            }
            private YObject getLocalVar(int index){
                return  stack.currentFrame().getLocalVariable(index);
            }
            private int popInt(){return stack.currentFrame().popOperand().toInteger();}
            private double popDouble(){return  stack.currentFrame().popOperand().toDouble();}
            private long popLong(){return stack.currentFrame().popOperand().toLong();}
            private float popFloat(){return  stack.currentFrame().popOperand().toFloat();}
            private YArray popArray(){return (YArray) stack.currentFrame().popOperand();}

            private void pushArray(YArray array) {
                stack.currentFrame().pushOperand(array);
            }
        }
        ConvenientDelegate dg = new ConvenientDelegate();//use a convenient delegate class to wrap stack operations


        /***************************************************************
         *  get the opcodes list from argument, which would be used soon
         *
         ***************************************************************/
        ArrayList<                                      //get opcode list from argument "op" of Opcode
                Tuple3<                                 //
                        Integer,                        //program counter
                        Integer,                        //opcode numeric value
                        Operand>>                       //operand of this opcode
                opcodes = op.getOpcodes();              //

        /***************************************************************
         *  create a critical section if it's a <synchronized> method
         *  and lock this region using a reentrant lock
         *
         ***************************************************************/
        if (isSynchronized) {
            methodLock = new ReentrantLock();
            methodLock.lock();
        }

        /***************************************************************
         *  create convenient class to handle exception table of this
         *  method
         *
         ***************************************************************/
        class ConvenientExceptionTableDelegate {
            private int findException(int programCounter, String x) {
                for (MetaClassMethod.ExceptionTable CatchType : exceptionTable) {
                    if (CatchType.catchTypeName.equals(x) && CatchType.startPC <= programCounter
                            && CatchType.endPC >= programCounter) {
                        return CatchType.handlePC;
                    }
                }
                return -1;
            }
        }
        ConvenientExceptionTableDelegate etDg = new ConvenientExceptionTableDelegate();


        /***************************************************************
         *  the real code execution part.
         *
         ***************************************************************/

        for (int i = 0; i < opcodes.size(); i++) {
            Tuple3 cd = opcodes.get(i);
            int programCount = (Integer) cd.get1Placeholder();
            thread.runtimeThread().pc(programCount);
            switch ((Integer) cd.get2Placeholder()) {

                //Load reference from array
                case Mnemonic.aaload: {
                    int index = dg.popInt();
                    YArray arrayRef = dg.popArray();

                    Continuation.ifNullThrowNullptrException(arrayRef);
                    Continuation.ifNotInThrowOutOfRangeException(arrayRef, index);

                    dg.push(arrayRef.get(index));
                }
                break;

                //Store into reference array
                case Mnemonic.aastore: {
                    YObject value = dg.pop();
                    int index = dg.popInt();
                    YArray array = dg.popArray();

                    array.set(index, value);
                }
                break;

                //Push null
                case Mnemonic.aconst_null: {
                    dg.push(null);
                }
                break;

                //Load reference from local variable
                case Mnemonic.aload: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject objectRef = dg.getLocalVar(index);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 0
                case Mnemonic.aload_0: {
                    YObject objectRef = dg.getLocalVar(0);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 1
                case Mnemonic.aload_1: {
                    YObject objectRef = dg.getLocalVar(1);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 2
                case Mnemonic.aload_2: {
                    YObject objectRef = dg.getLocalVar(2);
                    dg.push(objectRef);
                }
                break;

                //Load reference from local variable with index 3
                case Mnemonic.aload_3: {
                    YObject objectRef = dg.getLocalVar(3);
                    dg.push(objectRef);
                }
                break;

                //Create new array of reference
                case Mnemonic.anewarray: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int count = dg.popInt();

                    int index = (indexByte1 << 8) |
                            indexByte2;

                    String[] classes = (String[]) metaClassRef.constantPool.getClassNames().toArray();
                    if (!methodScopeRef.existClass(classes[index], classLoader.getClass())) {
                        YClassLoader loader = new YClassLoader();
                        loader.associateThread(thread);
                        try {
                            Tuple6 bundle = loader.loadClass(classes[index]);
                            MetaClass meta = loader.linkClass(bundle);
                            thread.runtimeVM().methodScope().addMetaClass(meta);
                            loader.loadInheritanceChain(meta.superClassName);
                            loader.initializeClass(meta);
                        } catch (ClassInitializingException | ClassLinkingException | ClassLoadingException e) {
                            throw new VMExecutionException("can not load class" + Peel.peelDescriptor(classes[index])
                                    + " while executing anewarray opcode");
                        }
                    }

                    YArray array = new YArray(count);
                    for (int t = 0; t < count; t++) {
                        YObject object = new YObject(methodScopeRef.getMetaClass(classes[index], classLoader.getClass()));
                        //object.initiateFields(classLoader);
                        array.set(t, object);
                    }

                    //add to runtime virtual machine heap section
                    thread.runtimeVM().heap().addToArrayArea(array);
                    //push reference to operand stack
                    dg.pushArray(array);

                }

                break;

                //Return reference from method
                case Mnemonic.areturn: {
                    YObject objectRef = dg.pop();

                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]

                    dg.clear();
                    stack.popFrame();
                    stack.currentFrame().pushOperand(objectRef);
                    return;
                }


                //Get length of array
                case Mnemonic.arraylength: {
                    YArray arrayRef = dg.popArray();
                    Continuation.ifNullThrowNullptrException(arrayRef);
                    dg.push(YObject.derivedFrom(arrayRef.getLength()));
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject top = dg.pop();

                    dg.setLocalVar(index, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_0: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject top = dg.pop();

                    dg.setLocalVar(0, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_1: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject top = dg.pop();

                    dg.setLocalVar(1, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_2: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject top = dg.pop();

                    dg.setLocalVar(2, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_3: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    YObject top = dg.pop();

                    dg.setLocalVar(3, top);
                }
                break;

                case Mnemonic.athrow: {
                    YObject object = dg.pop();

                    Continuation.ifNullThrowNullptrException(object);

                    class ThrowRoutine {
                        private int handleThrow() {
                            int handlePC = etDg.findException(programCount, object.getClassName());
                            if (handlePC != -1) {
                                Tuple3 newOp = opcodes.get(handlePC);
                                int currentI = opcodes.indexOf(newOp);
                                if (currentI == -1) {
                                    throw new VMExecutionException("incorrect address to go");
                                }
                                return currentI;
                            } else {
                                dg.clear();
                                dg.push(object);

                                stack.popFrame();
                                Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                                dg.push(object);
                                handleThrow();
                            }
                            return -1;  //should not reach here
                        }
                    }
                    ThrowRoutine throwRoutine = new ThrowRoutine();
                    i = throwRoutine.handleThrow();
                    if (i == -1) {
                        throw new VMExecutionException("execution sequence should not reach here");
                    }
                }
                break;

                //Load byte or boolean from array
                case Mnemonic.baload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();

                    Continuation.ifNullThrowNullptrException(array);

                    if (index > array.getLength()) {
                        throw new ArrayIndexOutOfBoundsException("array index out of bounds");
                    }
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.bastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    array.set(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.bipush: {
                    int x = ((GenericOperand) cd.get3Placeholder()).get0();
                    dg.push(YObject.derivedFrom(x));
                }
                break;

                case Mnemonic.caload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.castore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    array.set(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.checkcast: {
                    //todo:checkcast
                }
                break;

                case Mnemonic.d2f: {
                    double value = dg.popDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.d2i: {
                    double value = dg.popDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.d2l: {
                    double value = dg.popDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dadd: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(YObject.derivedFrom(value1 + value2));
                }
                break;

                case Mnemonic.daload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.dastore: {
                    double value = dg.popDouble();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    array.set(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dcmpg:
                case Mnemonic.dcmpl: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    float value1$ = (float) value1;
                    float value2$ = (float) value2;
                    if (value1$ > value2$) {
                        dg.push(YObject.derivedFrom(1));
                    } else if (value1$ < value2$) {
                        dg.push(YObject.derivedFrom(-1));
                    } else if ((Math.abs(value1$ - value2$) > 0)) {
                        dg.push(YObject.derivedFrom(0));
                    }
                }
                break;

                case Mnemonic.dconst_0: {
                    dg.push(YObject.derivedFrom(0.0D));
                }
                break;

                case Mnemonic.dconst_1: {
                    dg.push(YObject.derivedFrom(1.0D));
                }
                break;

                case Mnemonic.ddiv: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(YObject.derivedFrom(value1 / value2));
                }
                break;

                case Mnemonic.dload: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    double value = dg.getLocalVar(index).toDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dload_0: {
                    double value = dg.getLocalVar(0).toDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dload_1: {
                    double value = dg.getLocalVar(1).toDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dload_2: {
                    double value = dg.getLocalVar(2).toDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dload_3: {
                    double value = dg.getLocalVar(3).toDouble();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dmul: {
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(YObject.derivedFrom(value1 * value2));
                }

                case Mnemonic.dneg:{
                    double value = dg.popDouble();
                    dg.push(YObject.derivedFrom(-value));
                }
                break;

                case Mnemonic.drem:{
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(YObject.derivedFrom(value1 % value2));
                }
                break;

                //Return double from method
                case Mnemonic.dreturn:{
                    double value = dg.popDouble();

                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]
                    dg.clear();
                    stack.popFrame();
                    stack.currentFrame().pushOperand(YObject.derivedFrom(value));
                    return;
                }

                case Mnemonic.dstore:{
                    double value = dg.popDouble();
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    dg.setLocalVar(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dstore_0:{
                    double value = dg.popDouble();
                    dg.setLocalVar(0, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dstore_1:{
                    double value = dg.popDouble();
                    dg.setLocalVar(1, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dstore_2:{
                    double value = dg.popDouble();
                    dg.setLocalVar(2, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dstore_3:{
                    double value = dg.popDouble();
                    dg.setLocalVar(3, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.dsub:{
                    double value2 = dg.popDouble();
                    double value1 = dg.popDouble();
                    dg.push(YObject.derivedFrom(value1 - value2));
                }
                break;

                case Mnemonic.dup:{
                    YObject value = dg.pop();
                    dg.push(value);
                    dg.push(value);
                }
                break;

                case Mnemonic.dup_x1:{
                    YObject value1 = dg.pop();
                    YObject value2 = dg.pop();
                    dg.push(value1);
                    dg.push(value2);
                    dg.push(value1);
                }
                break;

                case Mnemonic.dup_x2:{
                    YObject value1 = dg.pop();
                    YObject value2 = dg.pop();
                    if (Predicate.isCategory2ComputationalType(value2)) {
                        //category 2 computational type
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        //category 1 computational type
                        YObject value3 = dg.pop();
                        dg.push(value1);
                        dg.push(value3);
                        dg.push(value2);
                        dg.push(value1);
                    }
                }
                break;

                case Mnemonic.dup2: {
                    YObject value = dg.pop();
                    if (Predicate.isCategory2ComputationalType(value)) {
                        //category 2 computational type
                        dg.push(value);
                        dg.push(value);
                    }else{
                        //category 1 computational type
                        YObject value2 = dg.pop();
                        dg.push(value2);
                        dg.push(value);
                        dg.push(value2);
                        dg.push(value);
                    }
                }
                break;

                case Mnemonic.dup2_x1:{
                    YObject value1 = dg.pop();
                    if (Predicate.isCategory2ComputationalType(value1)) {
                        //category 2 computational type
                        YObject value2 = dg.pop();
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        //category 1 computational type
                        YObject value2 = dg.pop();
                        YObject value3 = dg.pop();
                        dg.push(value2);
                        dg.push(value1);
                        dg.push(value3);
                        dg.push(value2);
                        dg.push(value1);
                    }
                }
                break;

                case Mnemonic.dup2_x2:{
                    YObject value1 = dg.pop();
                    YObject value2 = dg.pop();
                    //Form 4
                    if (Predicate.isCategory2ComputationalType(value1) &&
                            Predicate.isCategory2ComputationalType(value2)) {
                        dg.push(value1);
                        dg.push(value2);
                        dg.push(value1);
                    }else{
                        YObject value3 = dg.pop();
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
                            YObject value4 = dg.pop();
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
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.f2i:{
                    float value = dg.popFloat();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.f2l:{
                    float value = dg.popFloat();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fadd:{
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(YObject.derivedFrom(value1 + value2));
                }
                break;

                case Mnemonic.faload:{
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.fastore:{
                    float value = dg.popFloat();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    array.set(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fcmpg:
                case Mnemonic.fcmpl:{
                    float value2 = dg.popFloat();
                    float value1  = dg.popFloat();
                    if(value1 > value2){
                        dg.push(YObject.derivedFrom(1));
                    }else if(value1 < value2){
                        dg.push(YObject.derivedFrom(-1));
                    }else if(Math.abs(value1 - value2) > 0){
                        dg.push(YObject.derivedFrom(0));
                    }
                }
                break;

                case Mnemonic.fconst_0:{
                    dg.push(YObject.derivedFrom(0.0D));
                }
                break;

                case Mnemonic.fconst_1:{
                    dg.push(YObject.derivedFrom(1.0D));
                }
                break;

                case Mnemonic.fconst_2:{
                    dg.push(YObject.derivedFrom(2.0D));
                }
                break;

                case Mnemonic.fdiv:{
                    float value2 = dg.popFloat();
                    float value1  = dg.popFloat();
                    dg.push(YObject.derivedFrom(value1 / value2));
                }
                break;

                case Mnemonic.fload:{
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
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
                    dg.push(YObject.derivedFrom(value1 * value2));
                }
                break;

                case Mnemonic.fneg:{
                    float value = dg.popFloat();
                    dg.push(YObject.derivedFrom(-value));
                }
                break;

                case Mnemonic.frem:{
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(YObject.derivedFrom(value1 - (value1 / value2)));
                }
                break;

                //Return float from method
                case Mnemonic.freturn:{
                    float value = dg.popFloat();

                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]
                    dg.clear();
                    stack.popFrame();
                    stack.currentFrame().pushOperand(YObject.derivedFrom(value));
                    return;
                }

                case Mnemonic.fstore:{
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    float value = dg.popFloat();
                    dg.setLocalVar(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fstore_0: {
                    float value = dg.popFloat();
                    dg.setLocalVar(0, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fstore_1: {
                    float value = dg.popFloat();
                    dg.setLocalVar(1, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fstore_2: {
                    float value = dg.popFloat();
                    dg.setLocalVar(2, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fstore_3: {
                    float value = dg.popFloat();
                    dg.setLocalVar(3, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.fsub: {
                    float value2 = dg.popFloat();
                    float value1 = dg.popFloat();
                    dg.push(YObject.derivedFrom(value1 - value2));
                }
                break;

                //Fetch field from object
                case Mnemonic.getfield: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;

                    YObject object = dg.pop();
                    YObject field = object.getField(index);
                    if (!field.isInitialized()) {
                        field.initiateFields(classLoader);
                    }
                    dg.push(field);
                }
                break;

                //Get static field from class
                case Mnemonic.getstatic: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;

                    YObject staticVar = metaClassRef.getStaticVariable().get(index).get5Placeholder();
                    if (!staticVar.isInitialized()) {
                        staticVar.initiateFields(classLoader);
                    }

                    dg.push(staticVar);
                }
                break;

                case Mnemonic.goto$: {
                    int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int branchOffset = (branchByte1 << 8) | branchByte2;
                    Tuple3 newOp = opcodes.get(branchOffset);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = --currentI;
                }
                break;

                case Mnemonic.goto_w: {
                    /*
                    Although the goto_w instruction takes a 4-byte branch offset, other
                    factors limit the size of a method to 65535 bytes . This limit may
                    be raised in a future release of the Java Virtual Machine.
                     */
                    int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int branchByte3 = ((GenericOperand) cd.get3Placeholder()).get2();
                    int branchByte4 = ((GenericOperand) cd.get3Placeholder()).get3();
                    int branchOffset = (branchByte1 << 24) | (branchByte2 << 16)
                            | (branchByte3 << 8) | branchByte4;
                    Tuple3 newOp = opcodes.get(branchOffset);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = --currentI;
                }
                break;

                case Mnemonic.i2b: {
                    int value = dg.popInt();
                    byte value$ = (byte) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.i2c: {
                    int value = dg.popInt();
                    char value$ = (char) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.i2d: {
                    int value = dg.popInt();
                    double value$ = (double) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.i2f: {
                    int value = dg.popInt();
                    float value$ = (float) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.i2l: {
                    int value = dg.popInt();
                    long value$ = (long) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.i2s: {
                    int value = dg.popInt();
                    short value$ = (short) value;
                    dg.push(YObject.derivedFrom(value$));
                }
                break;

                case Mnemonic.iadd: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 + value2));
                }
                break;

                case Mnemonic.iaload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.iand: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 & value2));
                }
                break;

                case Mnemonic.iastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    array.set(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.iconst_m1: {
                    dg.push(YObject.derivedFrom(-1));
                }
                break;

                case Mnemonic.iconst_0: {
                    dg.push(YObject.derivedFrom(0));
                }
                break;

                case Mnemonic.iconst_1: {
                    dg.push(YObject.derivedFrom(1));
                }
                break;

                case Mnemonic.iconst_2: {
                    dg.push(YObject.derivedFrom(2));
                }
                break;

                case Mnemonic.iconst_3: {
                    dg.push(YObject.derivedFrom(3));
                }
                break;

                case Mnemonic.iconst_4: {
                    dg.push(YObject.derivedFrom(4));
                }
                break;

                case Mnemonic.iconst_5: {
                    dg.push(YObject.derivedFrom(5));
                }
                break;

                case Mnemonic.idiv: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }
                    dg.push(YObject.derivedFrom(value1 / value2));
                }
                break;

                case Mnemonic.if_acmpeq: {
                    YObject value2 = dg.pop();
                    YObject value1 = dg.pop();
                    if (value1 == value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_acmpne: {
                    YObject value2 = dg.pop();
                    YObject value1 = dg.pop();
                    if (value1 != value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpeq: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 == value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpne: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 != value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmplt: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 < value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpge: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 >= value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmpgt: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 > value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.if_icmple: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 <= value2) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifeq: {
                    int value = dg.popInt();
                    if (value == 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifne: {
                    int value = dg.popInt();
                    if (value != 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.iflt: {
                    int value = dg.popInt();
                    if (value < 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifge: {
                    int value = dg.popInt();
                    if (value >= 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifgt: {
                    int value = dg.popInt();
                    if (value > 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifle: {
                    int value = dg.popInt();
                    if (value <= 0) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifnonnull: {
                    YObject value = dg.pop();
                    if (value != null) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.ifnull: {
                    YObject value = dg.pop();
                    if (value == null) {
                        int branchByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                        int branchByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                        int branchOffset = (branchByte1 << 8) | branchByte2;
                        Tuple3 newOp = opcodes.get(branchOffset);
                        int currentI = opcodes.indexOf(newOp);
                        if (currentI == -1) {
                            throw new VMExecutionException("incorrect address to go");
                        }
                        i = --currentI;
                    }
                }
                break;

                case Mnemonic.iinc: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    int const$ = ((GenericOperand) cd.get3Placeholder()).get1();
                    dg.setLocalVar(index, YObject.derivedFrom(dg.getLocalVar(index).toInteger() + const$));
                }
                break;

                case Mnemonic.iload: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    int value = dg.getLocalVar(index).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.iload_0: {
                    int value = dg.getLocalVar(0).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.iload_1: {
                    int value = dg.getLocalVar(1).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.iload_2: {
                    int value = dg.getLocalVar(2).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.iload_3: {
                    int value = dg.getLocalVar(3).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.imul: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 * value2));
                }
                break;

                case Mnemonic.ineg: {
                    int value = dg.popInt();
                    dg.push(YObject.derivedFrom((~value) + 1));
                }
                break;

                case Mnemonic.instanceof$: {
                    //todo:instanceof
                }
                break;

                //Invoke dynamic method
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
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;

                    Tuple3 symbolicReference = metaClassRef.constantPool.findInSymbolicReference(index);

                    String methodBelongingClass = symbolicReference.get1Placeholder().toString();
                    if (!methodScopeRef.existClass(methodBelongingClass, classLoader.getClass())) {
                        YClassLoader loader = new YClassLoader();
                        loader.associateThread(thread);
                        try {
                            Tuple6 bundle = loader.loadClass(methodBelongingClass);
                            MetaClass meta = loader.linkClass(bundle);
                            thread.runtimeVM().methodScope().addMetaClass(meta);
                            loader.loadInheritanceChain(meta.superClassName);
                            loader.initializeClass(meta);
                        } catch (ClassInitializingException | ClassLinkingException | ClassLoadingException e) {
                            throw new VMExecutionException("can not load class" + Peel.peelDescriptor(methodBelongingClass)
                                    + " while executing anewarray opcode");
                        }
                    }

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
                    dg.push(YObject.derivedFrom(value1 | value2));
                }
                break;

                case Mnemonic.irem: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value2 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }
                    dg.push(YObject.derivedFrom(value1 - (value1 / value2)));
                }
                break;

                case Mnemonic.ireturn: {
                    int value = dg.popInt();

                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]
                    dg.clear();
                    stack.popFrame();
                    stack.currentFrame().pushOperand(YObject.derivedFrom(value));
                    return;
                }

                case Mnemonic.ishl: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 << (value2 & 0x1F)));
                }
                break;

                case Mnemonic.ishr: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 >> (value2 & 0x1F)));
                }
                break;

                case Mnemonic.istore: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    int value = dg.popInt();
                    dg.setLocalVar(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.istore_0: {
                    int value = dg.popInt();
                    dg.setLocalVar(0, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.istore_1: {
                    int value = dg.popInt();
                    dg.setLocalVar(1, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.istore_2: {
                    int value = dg.popInt();
                    dg.setLocalVar(2, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.istore_3: {
                    int value = dg.popInt();
                    dg.setLocalVar(3, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.isub: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 - value2));
                }
                break;

                case Mnemonic.iushr: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    if (value1 > 0) {

                        dg.push(YObject.derivedFrom(value1 >> (value2 & 0x1F)));
                    } else if (value1 < 0) {
                        dg.push(YObject.derivedFrom(value1 >> (value2 & 0x1F) + (1 << ~(value2 & 0x1F))));
                    }
                }
                break;

                case Mnemonic.ixor: {
                    int value2 = dg.popInt();
                    int value1 = dg.popInt();
                    dg.push(YObject.derivedFrom(value1 ^ value2));

                }
                break;

                case Mnemonic.jsr:
                case Mnemonic.jsr_w: {
                    throw new VMExecutionException("unsupport the <jsr/jsr_w> opcode, you may change a posterior compiler " +
                            "version of Java SE 6 ");
                }

                case Mnemonic.l2d: {
                    long value = dg.popLong();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.l2f: {
                    long value = dg.popLong();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.l2i: {
                    long value = dg.popLong();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.ladd: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 + value2));
                }
                break;

                case Mnemonic.laload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.push(array.get(index));
                }
                break;

                case Mnemonic.land: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 & value2));
                }
                break;

                case Mnemonic.lastore: {
                    long value = dg.popLong();
                    int index = dg.popInt();
                    YArray array = dg.popArray();
                    dg.setLocalVar(index, array.get(index));
                }
                break;

                case Mnemonic.lcmp: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    if (value1 > value2) {
                        dg.push(YObject.derivedFrom(1));
                    } else if (value1 == value2) {
                        dg.push(YObject.derivedFrom(0));
                    } else if (value1 < value2) {
                        dg.push(YObject.derivedFrom(-1));
                    }
                }
                break;

                case Mnemonic.lconst_0: {
                    dg.push(YObject.derivedFrom(0L));
                }
                break;

                case Mnemonic.lconst_1: {
                    dg.push(YObject.derivedFrom(1L));
                }
                break;

                //Push item from run-time constant pool
                case Mnemonic.ldc: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInFloat(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInFloat(index)));
                    } else if (!Predicate.isNull(poolRef.findInInteger(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInInteger(index)));
                    } else if (!Predicate.isNull(poolRef.findInString(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInString(index)));
                    } else if (!Predicate.isNull(poolRef.findInClass(index))) {

                    } else {
                        //todo:if not find class then load it
                        //todo: support methodtype and methodhandle
                    }
                }
                break;

                case Mnemonic.ldc_w: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int index = (indexByte1 << 8) | indexByte2;

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInFloat(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInFloat(index)));
                    } else if (!Predicate.isNull(poolRef.findInInteger(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInInteger(index)));
                    } else if (!Predicate.isNull(poolRef.findInString(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInString(index)));
                    } else if (!Predicate.isNull(poolRef.findInClass(index))) {

                    } else {
                        //todo:if not find class then load it
                        //todo: support methodtype and methodhandle
                    }
                }
                break;

                case Mnemonic.ldc2_w: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int index = (indexByte1 << 8) | indexByte2;

                    MetaClassConstantPool poolRef = metaClassRef.constantPool;

                    if (!Predicate.isNull(poolRef.findInLong(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInLong(index)));
                    } else if (!Predicate.isNull(poolRef.findInDouble(index))) {
                        dg.push(YObject.derivedFrom(poolRef.findInDouble(index)));
                    } else {
                        throw new VMExecutionException("ldc_2 has a invalid constant pool entry");
                    }
                }
                break;

                case Mnemonic.ldiv: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 / value2));
                }
                break;

                case Mnemonic.lload: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
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
                    dg.push(YObject.derivedFrom(value1 * value2));
                }
                break;

                case Mnemonic.lneg: {
                    long value = dg.popLong();
                    dg.push(YObject.derivedFrom(-value));
                }
                break;

                case Mnemonic.lookupswitch: {
                    int defaultGoto = ((TableSwitchOperand) cd.get3Placeholder()).get0();
                    HashMap<Integer, Integer> matchOffsetPairs = ((LookupSwitchOperand) cd.get3Placeholder()).get1();
                    int index = dg.popInt();
                    int newAddress = -1;
                    if (matchOffsetPairs.keySet().contains(index)) {
                        newAddress = matchOffsetPairs.get(index);
                    } else {
                        newAddress = programCount + defaultGoto;
                    }
                    Tuple3 newOp = opcodes.get(newAddress);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = --currentI;
                }
                break;

                case Mnemonic.lor: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 | value2));
                }
                break;

                case Mnemonic.lrem: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    if (value1 == 0) {
                        throw new ArithmeticException("the division is 0");
                    }

                    dg.push(YObject.derivedFrom(value1 - (value1 / value2) * value2));
                }
                break;

                case Mnemonic.lreturn: {
                    long value = dg.popLong();

                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]
                    dg.clear();
                    stack.popFrame();
                    stack.currentFrame().pushOperand(YObject.derivedFrom(value));
                    return;
                }

                case Mnemonic.lshl: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();

                    dg.push(YObject.derivedFrom(value1 << (value2 & 0x3F)));
                }
                break;

                case Mnemonic.lshr: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 >> (value2 & 0x3F)));
                }
                break;

                case Mnemonic.lstore: {
                    int index = ((GenericOperand) cd.get3Placeholder()).get0();
                    long value = dg.popLong();
                    dg.setLocalVar(index, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.lstore_0: {
                    long value = dg.popLong();
                    dg.setLocalVar(0, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.lstore_1: {
                    long value = dg.popLong();
                    dg.setLocalVar(1, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.lstore_2: {
                    long value = dg.popLong();
                    dg.setLocalVar(2, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.lstore_3: {
                    long value = dg.popLong();
                    dg.setLocalVar(3, YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.lsub: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 - value2));
                }
                break;

                case Mnemonic.lushr: {
                    int value2 = dg.popInt();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom((value1 >> (value2 & 0x3F)) + (2L << ~(value2 & 0x3F))));
                }
                break;

                case Mnemonic.lxor: {
                    long value2 = dg.popLong();
                    long value1 = dg.popLong();
                    dg.push(YObject.derivedFrom(value1 ^ value2));
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
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int dimensions = ((GenericOperand) cd.get3Placeholder()).get2();
                    int index = (indexByte1 << 8) |
                            indexByte2;

                    String[] classes = (String[]) metaClassRef.constantPool.getClassNames().toArray();
                    if (!methodScopeRef.existClass(classes[index], classLoader.getClass())) {
                        YClassLoader loader = new YClassLoader();
                        loader.associateThread(thread);
                        try {
                            Tuple6 bundle = loader.loadClass(classes[index]);
                            MetaClass meta = loader.linkClass(bundle);
                            thread.runtimeVM().methodScope().addMetaClass(meta);
                            loader.loadInheritanceChain(meta.superClassName);
                            loader.initializeClass(meta);
                        } catch (ClassInitializingException | ClassLinkingException | ClassLoadingException e) {
                            throw new VMExecutionException("can not load class" + Peel.peelDescriptor(classes[index])
                                    + " while executing anewarray opcode");
                        }
                    }

                    YArray array = new YArray(dimensions);
                    for (int t = 0; t < array.getLength(); t++) {
                        YArray subArray = new YArray(dg.popInt());
                        for (int m = 0; m < subArray.getLength(); m++) {
                            YObject object = new YObject(methodScopeRef.getMetaClass(classes[index], classLoader.getClass()));
                            //object.initiateFields(classLoader);
                            subArray.set(m, object);
                        }
                        array.set(t, subArray);
                    }

                    //add to runtime virtual machine heap section
                    thread.runtimeVM().heap().addToArrayArea(array);
                    //push reference to operand stack
                    dg.pushArray(array);
                }
                break;

                case Mnemonic.new$: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int count = dg.popInt();

                    int index = (indexByte1 << 8) |
                            indexByte2;

                    String[] classes = (String[]) metaClassRef.constantPool.getClassNames().toArray();
                    if (!methodScopeRef.existClass(classes[index], classLoader.getClass())) {
                        YClassLoader loader = new YClassLoader();
                        loader.associateThread(thread);
                        try {
                            Tuple6 bundle = loader.loadClass(classes[index]);
                            MetaClass meta = loader.linkClass(bundle);
                            thread.runtimeVM().methodScope().addMetaClass(meta);
                            loader.loadInheritanceChain(meta.superClassName);
                            loader.initializeClass(meta);
                        } catch (ClassInitializingException | ClassLinkingException | ClassLoadingException e) {
                            throw new VMExecutionException("can not load class" + Peel.peelDescriptor(classes[index])
                                    + " while executing anewarray opcode");
                        }
                    }

                    YObject object = new YObject(methodScopeRef.getMetaClass(classes[index], classLoader.getClass()));
                    object.initiateFields(classLoader);
                    thread.runtimeVM().heap().addToObjectArea(object);
                    dg.push(object);
                }
                break;

                case Mnemonic.newarray: {
                    int aType = ((GenericOperand) cd.get3Placeholder()).get0();
                    int count = dg.popInt();

                    YArray array = new YArray(count);
                    switch (aType) {
                        case NewArrayType.T_INT:
                        case NewArrayType.T_LONG:
                        case NewArrayType.T_SHORT:
                        case NewArrayType.T_BYTE:
                            for (int t = 0; t < count; t++) {
                                array.set(t, YObject.derivedFrom(0));
                            }
                            break;
                        case NewArrayType.T_CHAR:
                            for (int t = 0; t < count; t++) {
                                array.set(t, YObject.derivedFrom('\u0000'));
                            }
                            break;
                        case NewArrayType.T_DOUBLE:
                            for (int t = 0; t < count; t++) {
                                array.set(t, YObject.derivedFrom(0.0));
                            }
                            break;
                        case NewArrayType.T_FLOAT:
                            for (int t = 0; t < count; t++) {
                                array.set(t, YObject.derivedFrom(0.0F));
                            }
                            break;
                        case NewArrayType.T_BOOLEAN:
                            for (int t = 0; t < count; t++) {
                                array.set(t, YObject.derivedFrom(false));
                            }
                            break;
                        default:
                            throw new VMExecutionException("invalid operand " + aType + " of <newarray> opcode");
                    }

                    //add to runtime virtual machine heap section
                    thread.runtimeVM().heap().addToArrayArea(array);
                    //push reference to operand stack
                    dg.pushArray(array);
                }
                break;

                case Mnemonic.nop: {
                    //DO NOTHING :)
                }
                break;

                case Mnemonic.pop: {
                    dg.pop();
                }
                break;

                case Mnemonic.pop2: {
                    YObject value = dg.peek();
                    if (Predicate.isCategory2ComputationalType(value)) {
                        dg.pop();
                    } else {
                        YObject value1 = dg.pop();
                        YObject value2 = dg.peek();
                        if (Predicate.isCategory1ComputationalType(value2) &&
                                Predicate.isCategory1ComputationalType(value1)) {
                            dg.pop();
                        }
                    }
                }
                break;

                case Mnemonic.putfield: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;

                    YObject value = dg.pop();
                    YObject object = dg.pop();
                    if (!object.isInitialized()) {
                        object.initiateFields(classLoader);
                    }

                    Tuple3 fieldBundle = metaClassRef.constantPool.findInSymbolicReference(index);
                    switch (Peel.peelDescriptor((String) fieldBundle.get3Placeholder())) {
                        case "B":
                            object.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "C":
                            object.setField(index, YObject.derivedFrom(value.toChar()));
                            break;
                        case "D":
                            object.setField(index, YObject.derivedFrom(value.toDouble()));
                            break;
                        case "F":
                            object.setField(index, YObject.derivedFrom(value.toFloat()));
                            break;
                        case "I":
                            object.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "J":
                            object.setField(index, YObject.derivedFrom(value.toLong()));
                            break;
                        case "S":
                            object.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "Z":
                            object.setField(index, YObject.derivedFrom(value.toBoolean()));
                            break;
                        default:
                            object.setField(index, value);
                            break;
                    }
                }
                break;

                case Mnemonic.putstatic: {
                    int indexByte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int indexByte2 = ((GenericOperand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;


                    YObject value = dg.pop();
                    YObject staticVar = metaClassRef.getStaticVariable().get(index).get5Placeholder();
                    if (!staticVar.isInitialized()) {
                        staticVar.initiateFields(classLoader);
                    }

                    Tuple3 fieldBundle = metaClassRef.constantPool.findInSymbolicReference(index);
                    switch (Peel.peelDescriptor((String) fieldBundle.get3Placeholder())) {
                        case "B":
                            staticVar.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "C":
                            staticVar.setField(index, YObject.derivedFrom(value.toChar()));
                            break;
                        case "D":
                            staticVar.setField(index, YObject.derivedFrom(value.toDouble()));
                            break;
                        case "F":
                            staticVar.setField(index, YObject.derivedFrom(value.toFloat()));
                            break;
                        case "I":
                            staticVar.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "J":
                            staticVar.setField(index, YObject.derivedFrom(value.toLong()));
                            break;
                        case "S":
                            staticVar.setField(index, YObject.derivedFrom(value.toInteger()));
                            break;
                        case "Z":
                            staticVar.setField(index, YObject.derivedFrom(value.toBoolean()));
                            break;
                        default:
                            staticVar.setField(index, value);
                            break;
                    }
                }
                break;

                case Mnemonic.ret: {
                    throw new VMExecutionException("unsupport the <ret> opcode, you may change a posterior compiler " +
                            "version of Java SE 6 ");
                }

                case Mnemonic.return$: {
                    Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
                    //todo:check if the objectRef is corresponding to method return type;[enhance]
                    dg.clear();
                    stack.popFrame();
                    return;
                }

                case Mnemonic.saload: {
                    int index = dg.popInt();
                    YArray array = dg.popArray();

                    Continuation.ifNullThrowNullptrException(array);

                    short value = (short) array.get(index).toInteger();
                    dg.push(YObject.derivedFrom(value));
                }
                break;

                case Mnemonic.sastore: {
                    int value = dg.popInt();
                    int index = dg.popInt();
                    YArray array = dg.popArray();

                    Continuation.ifNullThrowNullptrException(array);

                    array.set(index, YObject.derivedFrom((short) value));
                }
                break;

                case Mnemonic.sipush: {
                    int byte1 = ((GenericOperand) cd.get3Placeholder()).get0();
                    int byte2 = ((GenericOperand) cd.get3Placeholder()).get0();
                    short byte$ = (short) ((byte1 <<
                            8) | byte2);
                    dg.push(YObject.derivedFrom(byte$));
                }
                break;

                case Mnemonic.swap: {
                    YObject value1 = dg.pop();
                    YObject value2 = dg.pop();
                    if (Predicate.isCategory1ComputationalType(value1) &&
                            Predicate.isCategory1ComputationalType(value2)) {
                        YObject temp = value1;
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
                    int defaultGoto = ((TableSwitchOperand) cd.get3Placeholder()).get0();
                    HashMap<Integer, Integer> jumpOffsets = ((TableSwitchOperand) cd.get3Placeholder()).get1();
                    int index = dg.popInt();
                    int newAddress = -1;
                    if (jumpOffsets.keySet().contains(index)) {
                        newAddress = jumpOffsets.get(index);
                    } else {
                        newAddress = programCount + defaultGoto;
                    }
                    Tuple3 newOp = opcodes.get(newAddress);
                    int currentI = opcodes.indexOf(newOp);
                    if (currentI == -1) {
                        throw new VMExecutionException("incorrect address to go");
                    }
                    i = --currentI;
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
        /***************************************************************
         *  leave a critical section if it's a <synchronized> method
         *  is denoted
         *
         ***************************************************************/
        Continuation.ifSynchronizedUnlock(methodLock, isSynchronized);
    }

    private void allocateStackFrame(int maxLocals, int maxStack) {
        YStackFrame frame = new YStackFrame();
        frame.allocateSize(maxStack, maxLocals);
        thread.runtimeThread().stack().pushFrame(frame);
    }
}
