package yvm.adt;

import common.Tuple3;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.function.Supplier;


public class Opcode {
    private u1[] codeRef;               //a reference which refer to codes array
    private int i;                      //special usage for simplify code;
    private ArrayList<                  //parsed opcodes array
            Tuple3<                     //
                    Integer,            //program counter
                    Integer,            //opcode value
                    Operand>>           //operand of related opcode
            opcodes;

    public Opcode(u1[] code) {
        codeRef = code;
        i = 0;
        opcodes = new ArrayList<>();
    }

    public void codes2Opcodes() throws ClassInitializingException {
        for (i = 0; i < codeRef.length; i++) {
            switch (codeRef[i].getValue()) {
                //Constants
                case Mnemonic.nop:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aconst_null:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_m1:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.iconst_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_4:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iconst_5:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lconst_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lconst_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fconst_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fconst_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fconst_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dconst_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dconst_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.bipush:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.sipush:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ldc:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.ldc_w:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ldc2_w:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                //Loads
                case Mnemonic.iload:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.lload:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.fload:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.dload:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.aload:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.iload_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iload_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iload_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iload_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lload_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lload_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lload_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lload_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fload_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fload_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fload_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fload_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dload_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dload_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dload_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dload_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aload_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aload_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aload_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aload_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iaload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.laload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.faload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.daload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aaload:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.baload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.caload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.saload:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //Stores
                case Mnemonic.istore:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.lstore:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.fstore:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.dstore:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.astore:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.istore_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.istore_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.istore_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.istore_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lstore_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lstore_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lstore_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lstore_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fstore_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fstore_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fstore_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fstore_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dstore_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dstore_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dstore_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dstore_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.astore_0:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.astore_1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.astore_2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.astore_3:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.dastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.aastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.bastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.castore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.sastore:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //Stack
                case Mnemonic.pop:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.pop2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup_x1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup_x2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup2_x1:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dup2_x2:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.swap:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //Math
                case Mnemonic.iadd:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ladd:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fadd:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dadd:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.isub:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lsub:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fsub:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dsub:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.imul:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lmul:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fmul:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dmul:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.idiv:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ldiv:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fdiv:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ddiv:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.irem:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lrem:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.frem:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.drem:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ineg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lneg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fneg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dneg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ishl:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lshl:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ishr:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lshr:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iushr:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lushr:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iand:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.land:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ior:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lor:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ixor:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lxor:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.iinc:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                //Conversions
                case Mnemonic.i2l:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.i2f:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.i2d:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.l2i:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.l2f:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.l2d:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.f2i:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.f2l:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.f2d:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.d2i:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.d2l:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.d2f:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.i2b:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.i2c:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.i2s:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //Comparisons
                case Mnemonic.lcmp:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fcmpl:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.fcmpg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dcmpl:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dcmpg:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.ifeq:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ifne:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.iflt:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ifge:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ifgt:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ifle:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmpeq:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmpne:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmplt:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmpge:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmpgt:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_icmple:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_acmpeq:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.if_acmpne:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                //Control
                case Mnemonic.goto$:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.jsr:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ret:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.tableswitch:
                    //3 bytes padding
                    nextCode();
                    nextCode();
                    nextCode();

                    addOpcode(codeRef[i].getValue(), this::newTableSwitchOperand);
                    break;
                case Mnemonic.lookupswitch:
                    //3 bytes padding
                    nextCode();
                    nextCode();
                    nextCode();

                    addOpcode(codeRef[i].getValue(), this::newLookupSwitchOperand);
                    break;
                case Mnemonic.ireturn:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.lreturn:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.freturn:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.dreturn:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.areturn:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.return$:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //References
                case Mnemonic.getstatic:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.putstatic:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.getfield:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.putfield:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.invokevirtual:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.invokespecial:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.invokestatic:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.invokeinterface:
                    addOpcode(codeRef[i].getValue(), this::newOperand4);
                    break;
                case Mnemonic.invokedynamic:
                    addOpcode(codeRef[i].getValue(), this::newOperand4);
                    break;
                case Mnemonic.new$:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.newarray:
                    addOpcode(codeRef[i].getValue(), this::newOperand1);
                    break;
                case Mnemonic.anewarray:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.arraylength:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.athrow:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.checkcast:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.instanceof$:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.monitorenter:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                case Mnemonic.monitorexit:
                    addOpcode(codeRef[i].getValue(), this::newOperand0);
                    break;
                //Extended
                case Mnemonic.wide:
                    switch (nextCode()) {
                        case Mnemonic.iload:
                        case Mnemonic.fload:
                        case Mnemonic.aload:
                        case Mnemonic.lload:
                        case Mnemonic.dload:
                        case Mnemonic.istore:
                        case Mnemonic.fstore:
                        case Mnemonic.astore:
                        case Mnemonic.lstore:
                        case Mnemonic.dstore:
                        case Mnemonic.ret:
                            addOpcode(codeRef[i].getValue(), this::newOperand2);
                            break;
                        case Mnemonic.iinc:
                            addOpcode(codeRef[i].getValue(), this::newOperand4);
                            break;
                        default:
                            throw new ClassInitializingException("unknown opcode found");
                    }
                    break;
                case Mnemonic.multianewarray:
                    addOpcode(codeRef[i].getValue(), this::newOperand3);
                    break;
                case Mnemonic.ifnull:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.ifnonnull:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.goto_w:
                    addOpcode(codeRef[i].getValue(), this::newOperand4);
                    break;
                case Mnemonic.jsr_w:
                    addOpcode(codeRef[i].getValue(), this::newOperand4);
                    break;
                //Reserved
                case Mnemonic.breakpoint:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.impdep1:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                case Mnemonic.impdep2:
                    addOpcode(codeRef[i].getValue(), this::newOperand2);
                    break;
                default:
                    throw new ClassInitializingException("unknown opcode found");
            }
        }
    }

    public ArrayList getOpcodes() {
        return opcodes;
    }

    public void debug(String thisClassName) {
        System.out.println("#" + thisClassName + " Opcodes#");
        opcodes.forEach((Elem) -> {
            Field[] fields = Mnemonic.class.getDeclaredFields();
            for (Field f : fields) {
                try {
                    if (f.getInt(Mnemonic.class) == Elem.get2Placeholder()) {
                        System.out.println(Elem.get1Placeholder() + " " + f.getName() + " " + Elem.get3Placeholder());
                    }
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    //Code simplify methods

    private void addOpcode(int opcodeValue, final Supplier<Operand> cons) {
        this.opcodes.add(new Tuple3<>(i, opcodeValue, cons.get()));
    }

    private Operand newLookupSwitchOperand() {
        LookupSwitchOperand operand = new LookupSwitchOperand();
        int defaultGoto = bytes2int(
                new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
        operand.setDefaultGoto(defaultGoto);
        int pairsNum = bytes2int(
                new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
        for (int i = 0; i < pairsNum; i++) {
            int match = bytes2int(
                    new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
            int offset = bytes2int(
                    new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
            operand.addMatchOffsetPair(match, offset);
        }
        return operand;
    }

    private Operand newTableSwitchOperand() {
        TableSwitchOperand operand = new TableSwitchOperand();
        int defaultGoto = bytes2int(
                new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
        operand.setDefaultGoto(defaultGoto);
        int low = bytes2int(
                new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
        int high = bytes2int(
                new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
        for (int i = 0; i < high - low + 1; i++) {
            int offset = bytes2int(
                    new byte[]{(byte) nextCode(), (byte) nextCode(), (byte) nextCode(), (byte) nextCode()});
            operand.addJumpOffset(low + i, offset);
        }
        return operand;
    }

    private Operand newOperand0() {
        return GenericOperand.new$();
    }

    private Operand newOperand1() {
        return GenericOperand.new$(nextCode());
    }

    private Operand newOperand2() {
        return GenericOperand.new$(nextCode(), nextCode());
    }

    private Operand newOperand3() {
        return GenericOperand.new$(nextCode(), nextCode(), nextCode());
    }

    private Operand newOperand4() {
        return GenericOperand.new$(nextCode(), nextCode(), nextCode(), nextCode());
    }

    private int nextCode() {
        return codeRef[++i].getValue();
    }

    private int bytes2int(byte[] bs) {
        int value;
        value = (bs[0] & 0xFF)
                | ((bs[1] & 0xFF) << 8)
                | ((bs[2] & 0xFF) << 16)
                | ((bs[3] & 0xFF) << 24);
        return value;
    }
}
