package yvm.adt;

import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.function.Supplier;


interface Mnemonic {
    //Constants
    int nop = 0;
    int aconst_null = 1;
    int iconst_m1 = 2;
    int iconst_0 = 3;
    int iconst_1 = 4;
    int iconst_2 = 5;
    int iconst_3 = 6;
    int iconst_4 = 7;
    int iconst_5 = 8;
    int lconst_0 = 9;
    int lconst_1 = 10;
    int fconst_0 = 11;
    int fconst_1 = 12;
    int fconst_2 = 13;
    int dconst_0 = 14;
    int dconst_1 = 15;
    int bipush = 16;
    int sipush = 17;
    int ldc = 18;
    int ldc_w = 19;
    int ldc2_w = 20;
    //Loads
    int iload = 21;
    int lload = 22;
    int fload = 23;
    int dload = 24;
    int aload = 25;
    int iload_0 = 26;
    int iload_1 = 27;
    int iload_2 = 28;
    int iload_3 = 29;
    int lload_0 = 30;
    int lload_1 = 31;
    int lload_2 = 32;
    int lload_3 = 33;
    int fload_0 = 34;
    int fload_1 = 35;
    int fload_2 = 36;
    int fload_3 = 37;
    int dload_0 = 38;
    int dload_1 = 39;
    int dload_2 = 40;
    int dload_3 = 41;
    int aload_0 = 42;
    int aload_1 = 43;
    int aload_2 = 44;
    int aload_3 = 45;
    int iaload = 46;
    int laload = 47;
    int faload = 48;
    int daload = 49;
    int aaload = 50;
    int baload = 51;
    int caload = 52;
    int saload = 53;
    //Stores
    int istore = 54;
    int lstore = 55;
    int fstore = 56;
    int dstore = 57;
    int astore = 58;
    int istore_0 = 59;
    int istore_1 = 60;
    int istore_2 = 61;
    int istore_3 = 62;
    int lstore_0 = 63;
    int lstore_1 = 64;
    int lstore_2 = 65;
    int lstore_3 = 66;
    int fstore_0 = 67;
    int fstore_1 = 68;
    int fstore_2 = 69;
    int fstore_3 = 70;
    int dstore_0 = 71;
    int dstore_1 = 72;
    int dstore_2 = 73;
    int dstore_3 = 74;
    int astore_0 = 75;
    int astore_1 = 76;
    int astore_2 = 77;
    int astore_3 = 78;
    int iastore = 79;
    int lastore = 80;
    int fastore = 81;
    int dastore = 82;
    int aastore = 83;
    int bastore = 84;
    int castore = 85;
    int sastore = 86;
    //Stack
    int pop = 87;
    int pop2 = 88;
    int dup = 89;
    int dup_x1 = 90;
    int dup_x2 = 91;
    int dup2 = 92;
    int dup2_x1 = 93;
    int dup2_x2 = 94;
    int swap = 95;
    //Math
    int iadd = 96;
    int ladd = 97;
    int fadd = 98;
    int dadd = 99;
    int isub = 100;
    int lsub = 101;
    int fsub = 102;
    int dsub = 103;
    int imul = 104;
    int lmul = 105;
    int fmul = 106;
    int dmul = 107;
    int idiv = 108;
    int ldiv = 109;
    int fdiv = 110;
    int ddiv = 111;
    int irem = 112;
    int lrem = 113;
    int frem = 114;
    int drem = 115;
    int ineg = 116;
    int lneg = 117;
    int fneg = 118;
    int dneg = 119;
    int ishl = 120;
    int lshl = 121;
    int ishr = 122;
    int lshr = 123;
    int iushr = 124;
    int lushr = 125;
    int iand = 126;
    int land = 127;
    int ior = 128;
    int lor = 129;
    int ixor = 130;
    int lxor = 131;
    int iinc = 132;
    //Conversions
    int i2l = 133;
    int i2f = 134;
    int i2d = 135;
    int l2i = 136;
    int l2f = 137;
    int l2d = 138;
    int f2i = 139;
    int f2l = 140;
    int f2d = 141;
    int d2i = 142;
    int d2l = 143;
    int d2f = 144;
    int i2b = 145;
    int i2c = 146;
    int i2s = 147;
    //Comparisons
    int lcmp = 148;
    int fcmpl = 149;
    int fcmpg = 150;
    int dcmpl = 151;
    int dcmpg = 152;
    int ifeq = 153;
    int ifne = 154;
    int iflt = 155;
    int ifge = 156;
    int ifgt = 157;
    int ifle = 158;
    int if_icmpeq = 159;
    int if_icmpne = 160;
    int if_icmplt = 161;
    int if_icmpge = 162;
    int if_icmpgt = 163;
    int if_icmple = 164;
    int if_acmpeq = 165;
    int if_acmpne = 166;
    //Control
    int goto$ = 167;
    int jsr = 168;
    int ret = 169;
    int tableswitch = 170;
    int lookupswitch = 171;
    int ireturn = 172;
    int lreturn = 173;
    int freturn = 174;
    int dreturn = 175;
    int areturn = 176;
    int return$ = 177;
    //References
    int getstatic = 178;
    int putstatic = 179;
    int getfield = 180;
    int putfield = 181;
    int invokevirtual = 182;
    int invokespecial = 183;
    int invokestatic = 184;
    int invokeinterface = 185;
    int invokedynamic = 186;
    int new$ = 187;
    int newarray = 188;
    int anewarray = 189;
    int arraylength = 190;
    int athrow = 191;
    int checkcast = 192;
    int instanceof$ = 193;
    int monitorenter = 194;
    int monitorexit = 195;
    //Extended
    int wide = 196;
    int multianewarray = 197;
    int ifnull = 198;
    int ifnonnull = 199;
    int goto_w = 200;
    int jsr_w = 201;
    //Reserved
    int breakpoint = 202;
    int impdep1 = 254;
    int impdep2 = 255;

}

interface Operand {

}

class GenericOperand implements Operand {
    private int[] operands;

    private GenericOperand() {
        operands = null;
    }

    static Operand new$(int... args) {
        GenericOperand op = new GenericOperand();
        op.operands = new int[args.length];
        for (int i = 0; i < args.length; i++) {
            op.operands[i] = args[i];
        }
        return op;
    }

}

class LookupSwitchOperand implements Operand {
    public HashMap<Integer, Integer> matchOffsetPairs;
    private int defaultGoto;

    public LookupSwitchOperand() {
        matchOffsetPairs = new HashMap<>();
    }

    public void setDefaultGoto(int defaultGoto) {
        this.defaultGoto = defaultGoto;
    }

    void addMatchOffsetPair(int match, int offset) {
        matchOffsetPairs.put(match, offset);
    }
}

class TableSwitchOperand implements Operand {
}

public class Opcode {
    private u1[] codeRef;               //a reference which refer to codes array
    private int i;                      //special usage for simplify code;
    private ArrayList<                  //parsed opcodes array
            Tuple3<                     //
                    Integer,            //index
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
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aconst_null:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_m1:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.iconst_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_4:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iconst_5:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lconst_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lconst_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fconst_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fconst_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fconst_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dconst_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dconst_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.bipush:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.sipush:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ldc:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.ldc_w:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ldc2_w:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                //Loads
                case Mnemonic.iload:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.lload:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.fload:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.dload:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.aload:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.iload_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iload_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iload_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iload_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lload_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lload_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lload_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lload_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fload_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fload_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fload_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fload_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dload_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dload_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dload_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dload_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aload_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aload_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aload_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aload_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iaload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.laload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.faload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.daload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aaload:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.baload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.caload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.saload:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                //Stores
                case Mnemonic.istore:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.lstore:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.fstore:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.dstore:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.astore:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.istore_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.istore_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.istore_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.istore_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lstore_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lstore_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lstore_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lstore_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fstore_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fstore_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fstore_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fstore_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dstore_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dstore_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dstore_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dstore_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.astore_0:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.astore_1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.astore_2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.astore_3:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fastore:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.dastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.aastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.bastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.castore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.sastore:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                //Stack
                case Mnemonic.pop:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.pop2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup_x1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup_x2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup2_x1:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dup2_x2:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.swap:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                //Math
                case Mnemonic.iadd:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ladd:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fadd:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dadd:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.isub:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lsub:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fsub:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dsub:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.imul:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lmul:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fmul:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dmul:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.idiv:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ldiv:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fdiv:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ddiv:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.irem:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lrem:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.frem:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.drem:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ineg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lneg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fneg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dneg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ishl:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lshl:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ishr:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lshr:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iushr:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lushr:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iand:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.land:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ior:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lor:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ixor:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lxor:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.iinc:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                //Conversions
                case Mnemonic.i2l:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.i2f:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.i2d:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.l2i:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.l2f:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.l2d:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.f2i:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.f2l:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.f2d:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.d2i:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.d2l:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.d2f:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.i2b:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.i2c:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.i2s:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                //Comparisons
                case Mnemonic.lcmp:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fcmpl:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.fcmpg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dcmpl:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dcmpg:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.ifeq:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ifne:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.iflt:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ifge:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ifgt:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ifle:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmpeq:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmpne:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmplt:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmpge:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmpgt:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_icmple:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_acmpeq:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.if_acmpne:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                //Control
                case Mnemonic.goto$:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.jsr:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ret:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.tableswitch:
                    //3 bytes padding
                    nextCode();
                    nextCode();
                    nextCode();

                    addOpcode(codeRef[i].getValue(), this::new_tableswitch_operand);
                    break;
                case Mnemonic.lookupswitch:
                    //3 bytes padding
                    nextCode();
                    nextCode();
                    nextCode();

                    addOpcode(codeRef[i].getValue(), this::new_lookupswitch_operand);
                    break;
                case Mnemonic.ireturn:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.lreturn:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.freturn:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.dreturn:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.areturn:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.return$:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                //References
                case Mnemonic.getstatic:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.putstatic:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.getfield:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.putfield:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.invokevirtual:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.invokespecial:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.invokestatic:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.invokeinterface:
                    addOpcode(codeRef[i].getValue(), this::new_4_operand);
                    break;
                case Mnemonic.invokedynamic:
                    addOpcode(codeRef[i].getValue(), this::new_4_operand);
                    break;
                case Mnemonic.new$:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.newarray:
                    addOpcode(codeRef[i].getValue(), this::new_1_operand);
                    break;
                case Mnemonic.anewarray:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.arraylength:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.athrow:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.checkcast:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.instanceof$:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.monitorenter:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
                    break;
                case Mnemonic.monitorexit:
                    addOpcode(codeRef[i].getValue(), this::new_0_operand);
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
                            addOpcode(codeRef[i].getValue(), this::new_2_operand);
                            break;
                        case Mnemonic.iinc:
                            addOpcode(codeRef[i].getValue(), this::new_4_operand);
                            break;
                        default:
                            throw new ClassInitializingException("unknown opcode found");
                    }
                    break;
                case Mnemonic.multianewarray:
                    addOpcode(codeRef[i].getValue(), this::new_3_operand);
                    break;
                case Mnemonic.ifnull:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.ifnonnull:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.goto_w:
                    addOpcode(codeRef[i].getValue(), this::new_4_operand);
                    break;
                case Mnemonic.jsr_w:
                    addOpcode(codeRef[i].getValue(), this::new_4_operand);
                    break;
                //Reserved
                case Mnemonic.breakpoint:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.impdep1:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
                case Mnemonic.impdep2:
                    addOpcode(codeRef[i].getValue(), this::new_2_operand);
                    break;
            }
        }
    }


    //Code simplify methods

    private void addOpcode(int opcodeValue, final Supplier<Operand> cons) {
        this.opcodes.add(new Tuple3<>(i, opcodeValue, cons.get()));
    }

    private Operand new_lookupswitch_operand() {
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

    private Operand new_tableswitch_operand() {
        TableSwitchOperand operand = new TableSwitchOperand();
        return operand;
    }

    private Operand new_0_operand() {
        return GenericOperand.new$();
    }

    private Operand new_1_operand() {
        return GenericOperand.new$(nextCode());
    }

    private Operand new_2_operand() {
        return GenericOperand.new$(nextCode(), nextCode());
    }

    private Operand new_4_operand() {
        return GenericOperand.new$(nextCode(), nextCode(), nextCode(), nextCode());
    }

    private Operand new_3_operand() {
        return GenericOperand.new$(nextCode(), nextCode(), nextCode());
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
