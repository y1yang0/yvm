package yvm.adt;

import static yvm.auxil.Convert.rawArrayToString;

public class GenericOperand implements Operand<Integer, Integer, Integer, Integer> {
    private int[] operands;

    private GenericOperand() {
        operands = null;
    }

    static Operand new$(int... args) {
        GenericOperand op = new GenericOperand();
        op.operands = new int[args.length];
        System.arraycopy(args, 0, op.operands, 0, args.length);
        return op;
    }

    @Override
    public String toString() {
        return rawArrayToString(operands, ',');
    }

    @Override
    public Integer get0() {
        return operands[0];
    }

    @Override
    public Integer get1() {
        return operands[1];
    }

    @Override
    public Integer get2() {
        return operands[2];
    }

    @Override
    public Integer get3() {
        return operands[3];
    }
}
