package yvm.exec;

import rtstruct.*;
import rtstruct.meta.MetaClass;
import rtstruct.meta.MetaClassMethod;
import rtstruct.ystack.YStack;
import rtstruct.ystack.YStackFrame;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import yvm.adt.*;
import yvm.auxil.Peel;

import java.util.ArrayList;

import static java.util.Objects.isNull;
import static yvm.auxil.Predicate.*;

public final class CodeExecutionEngine {
    private YThread thread;
    private boolean ignited;
    private MetaClass metaClassRef;
    private YMethodScope methodScopeRef;
    private YHeap heapRef;

    public CodeExecutionEngine(YThread thread) {
        this.thread = thread;
        ignited = false;
    }

    public void ignite(MetaClass metaClassInfo) {
        this.metaClassRef = metaClassInfo;
        ignited = true;
    }

    public void associateMethodScope(YMethodScope methodScope) {
        methodScopeRef = methodScope;
    }

    public void associateHeap(YHeap heap) {
        heapRef = heap;
    }

    public void executeCLinit() throws ClassInitializingException {
        if (!ignited) {
            throw new ClassInitializingException("code execution engine is not ready");
        }

        Tuple6<String, String, u1[], MetaClassMethod.StackRequirement,
                MetaClassMethod.ExceptionTable[], ArrayList<Attribute>>
                clinit = metaClassRef.getMethods().findMethod("<clinit>");

        if (isNull(clinit) || strNotEqual(clinit.get1Placeholder(), "<clinit>")) {
            throw new ClassInitializingException("can not find synthetic <clinit> method");
        }

        int maxLocals = clinit.get4Placeholder().maxLocals;
        int maxStack = clinit.get4Placeholder().maxStack;

        YStackFrame frame = new YStackFrame();
        frame.allocateSize(maxStack, maxLocals);
        thread.stack().pushFrame(frame);

        Opcode op = new Opcode(clinit.get3Placeholder());
        op.codes2Opcodes();
        op.debug(metaClassRef.getQualifiedClassName() + " clinit");
        //codeExecution(op);
    }

    @SuppressWarnings("unused")
    private void codeExecution(Opcode op) throws ClassInitializingException {
        YStack stack = thread.stack();
        class Aux {
            private Object pop() {
                return stack.currentFrame().pop$operand();
            }

            private void push(Object object) {
                stack.currentFrame().push$operand(object);
            }
        }

        Aux aux = new Aux();
        //program counter//opcode value//operand of related opcode
        ArrayList<Tuple3<Integer, Integer, Operand>>
                opcodes = op.getOpcodes();
        for (int i = 0; i < opcodes.size(); i++) {
            Tuple3 cd = opcodes.get(i);
            int programCount = (Integer) cd.get1Placeholder();
            thread.pc(programCount);
            switch ((Integer) cd.get2Placeholder()) {

                //Load reference from array
                case Mnemonic.aaload: {
                    int index = (int) aux.pop();
                    YArray arrayRef = (YArray) aux.pop();

                    if (isNull(arrayRef)) {
                        throw new NullPointerException("reference of an array is null");
                    }
                    if (!inRange(arrayRef, index)) {
                        throw new ArrayIndexOutOfBoundsException("array index " + index + " out of bounds");
                    }
                    aux.push(arrayRef.get(index));
                }
                break;

                //Store into reference array
                case Mnemonic.aastore: {
                    Object value = aux.pop();
                    int index = (int) aux.pop();
                    YArray arrayRef = (YArray) aux.pop();

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
                    aux.push(null);
                }
                break;

                //Load reference from local variable
                case Mnemonic.aload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object objectRef = stack.currentFrame().getLocalVariable(index);
                    aux.push(objectRef);
                }
                break;

                //Load reference from local variable with index 0
                case Mnemonic.aload_0: {
                    Object objectRef = stack.currentFrame().getLocalVariable(0);
                    aux.push(objectRef);
                }
                break;

                //Load reference from local variable with index 1
                case Mnemonic.aload_1: {
                    Object objectRef = stack.currentFrame().getLocalVariable(1);
                    aux.push(objectRef);
                }
                break;

                //Load reference from local variable with index 2
                case Mnemonic.aload_2: {
                    Object objectRef = stack.currentFrame().getLocalVariable(2);
                    aux.push(objectRef);
                }
                break;

                //Load reference from local variable with index 3
                case Mnemonic.aload_3: {
                    Object objectRef = stack.currentFrame().getLocalVariable(3);
                    aux.push(objectRef);
                }
                break;

                //Create new array of reference
                case Mnemonic.anewarray: {
                    //The count represents the number of components of the array to
                    //be created.
                    int count = (int) aux.pop();
                    byte indexByte1 = (byte) ((Operand) cd.get3Placeholder()).get0();
                    byte indexByte2 = (byte) ((Operand) cd.get3Placeholder()).get1();
                    int index = (indexByte1 << 8) | indexByte2;
                    String res = metaClassRef.getConstantPool().findInClasses(index);

                    if (count < 0) {
                        throw new NegativeArraySizeException("array size required a positive integer");
                    }
                    if (!isNull(res)) {
                        if (methodScopeRef.existClass(res)) {
                            MetaClass meta = methodScopeRef.getMetaClass(res, metaClassRef.getClassLoader());
                            YObject object = new YObject(meta);
                            object.init();

                            YArray array = new YArray<>(count, object);
                            array.init();

                            aux.push(array);
                        } else {
                            String arrayComponentType = Peel.getArrayComponent(res);
                            int arrayDimension = Peel.getArrayDimension(res);
                            if (methodScopeRef.existClass(arrayComponentType)) {
                                MetaClass meta = methodScopeRef.getMetaClass(res, metaClassRef.getClassLoader());
                                YObject object = new YObject(meta);
                                object.init();

                                YArray inner = new YArray<>(arrayDimension, object);
                                inner.init();

                                YArray outer = new YArray<>(count, inner);
                                outer.init();

                                aux.push(outer);
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
                    YArray arrayRef = (YArray) aux.pop();
                    if (isNull(arrayRef)) {
                        throw new NullPointerException("array reference is null");
                    }
                    aux.push(arrayRef.getDimension());
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = aux.pop();

                    stack.currentFrame().setLocalVariable(index, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_0: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = aux.pop();

                    stack.currentFrame().setLocalVariable(0, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_1: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = aux.pop();

                    stack.currentFrame().setLocalVariable(1, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_2: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = aux.pop();

                    stack.currentFrame().setLocalVariable(2, top);
                }
                break;

                //Store reference into local variable
                case Mnemonic.astore_3: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    Object top = aux.pop();

                    stack.currentFrame().setLocalVariable(3, top);
                }
                break;

                case Mnemonic.athrow: {
                    //todo:athrow
                }
                break;

                //Load byte or boolean from array
                case Mnemonic.baload: {
                    int index = (int) aux.pop();
                    YArray arrayRef = (YArray) aux.pop();
                    if (isNull(arrayRef)) {
                        throw new NullPointerException("array reference is null");
                    }
                    if (index > arrayRef.getDimension()) {
                        throw new ArrayIndexOutOfBoundsException("array index out of bounds");
                    }
                    aux.push(arrayRef.get(index));
                }
                break;

                case Mnemonic.bastore: {
                    int value = (int) aux.pop();
                    int index = (int) aux.pop();
                    YArray array = (YArray) aux.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.bipush: {
                    aux.push(((Operand) cd.get3Placeholder()).get0());
                }
                break;

                case Mnemonic.caload: {
                    int index = (int) aux.pop();
                    YArray array = (YArray) aux.pop();
                    aux.push(array.get(index));
                }
                break;

                case Mnemonic.castore: {
                    int value = (int) aux.pop();
                    int index = (int) aux.pop();
                    YArray array = (YArray) aux.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.checkcast: {
                    //todo:checkcast
                }
                break;

                case Mnemonic.d2f: {
                    double value = (double) aux.pop();
                    aux.push((float) value);
                }
                break;

                case Mnemonic.d2i: {
                    double value = (double) aux.pop();
                    aux.push((int) value);
                }
                break;

                case Mnemonic.d2l: {
                    double value = (double) aux.pop();
                    aux.push((long) value);
                }
                break;

                case Mnemonic.dadd: {
                    double value2 = (double) aux.pop();
                    double value1 = (double) aux.pop();
                    aux.push(value1 + value2);
                }
                break;

                case Mnemonic.daload: {
                    int index = (int) aux.pop();
                    YArray array = (YArray) aux.pop();
                    aux.push(array.get(index));
                }
                break;

                case Mnemonic.dastore: {
                    double value = (double) aux.pop();
                    int index = (int) aux.pop();
                    YArray array = (YArray) aux.pop();
                    array.set(index, value);
                }
                break;

                case Mnemonic.dcmpg:
                case Mnemonic.dcmpl: {
                    double value2 = (double) aux.pop();
                    double value1 = (double) aux.pop();
                    float value1$ = (float) value1;
                    float value2$ = (float) value2;
                    if (value1$ > value2$) {
                        aux.push(0);
                    } else if (value1$ < value2$) {
                        aux.push(-1);
                    } else if ((Math.abs(value1$ - value2$) > 0)) {
                        aux.push(0);
                    }
                }
                break;

                case Mnemonic.dconst_0: {
                    aux.push(0.0);
                }
                break;

                case Mnemonic.dconst_1: {
                    aux.push(1.0);
                }
                break;

                case Mnemonic.ddiv: {
                    double value2 = (double) aux.pop();
                    double value1 = (double) aux.pop();
                    aux.push(value1 / value2);
                }
                break;

                case Mnemonic.dload: {
                    int index = (int) ((Operand) cd.get3Placeholder()).get0();
                    double value = (double) stack.currentFrame().getLocalVariable(index);
                    aux.push(value);
                }
                break;

                case Mnemonic.dload_0: {
                    double value = (double) stack.currentFrame().getLocalVariable(0);
                    aux.push(value);
                }
                break;

                case Mnemonic.dload_1: {
                    double value = (double) stack.currentFrame().getLocalVariable(1);
                    aux.push(value);
                }
                break;

                case Mnemonic.dload_2: {
                    double value = (double) stack.currentFrame().getLocalVariable(2);
                    aux.push(value);
                }
                break;

                case Mnemonic.dload_3: {
                    double value = (double) stack.currentFrame().getLocalVariable(3);
                    aux.push(value);
                }
                break;

                case Mnemonic.dmul: {
                    double value2 = (double) aux.pop();
                    double value1 = (double) aux.pop();
                    aux.push(value1 * value2);
                }


                default:
                    throw new ClassInitializingException("unknown opcode in execution sequence");
            }
        }
    }
}
