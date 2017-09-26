package yvm.adt;

import java.util.HashMap;

import static yvm.auxil.Convert.rawArrayToString;

public interface Operand<_FstPlaceHolderType, _SndPlaceHolderType> {
    _FstPlaceHolderType get0();

    _SndPlaceHolderType get1();
}

class GenericOperand implements Operand<Integer, Integer> {
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
}

class LookupSwitchOperand implements Operand<Integer, HashMap<Integer, Integer>> {
    private HashMap<Integer, Integer> matchOffsetPairs;
    private int defaultGoto;

    LookupSwitchOperand() {
        matchOffsetPairs = new HashMap<>();
    }

    void setDefaultGoto(int defaultGoto) {
        this.defaultGoto = defaultGoto;
    }

    void addMatchOffsetPair(int match, int offset) {
        matchOffsetPairs.put(match, offset);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("{default goto:").append(defaultGoto).append(",");
        sb.append("lookup table:");
        matchOffsetPairs.forEach(
                (Match, Offset) -> {
                    sb.append(Match + " " + Offset + "||");
                }
        );
        sb.append("}");
        return sb.toString();
    }

    @Override
    public Integer get0() {
        return defaultGoto;
    }

    @Override
    public HashMap<Integer, Integer> get1() {
        return matchOffsetPairs;
    }
}

class TableSwitchOperand implements Operand<Integer, HashMap<Integer, Integer>> {
    private int defaultGoto;
    private HashMap<Integer, Integer> jumpOffsets;

    TableSwitchOperand() {
        jumpOffsets = new HashMap<>();
    }

    void setDefaultGoto(int defaultGoto) {
        this.defaultGoto = defaultGoto;
    }

    void addJumpOffset(int match, int offset) {
        this.jumpOffsets.put(match, offset);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("{default goto:").append(defaultGoto).append(",");
        sb.append("table switch:");
        jumpOffsets.forEach(
                (Index, Offset) -> {
                    sb.append(Index).append(" ").append(Offset).append("||");
                }
        );
        sb.append("}");
        return sb.toString();
    }

    @Override
    public Integer get0() {
        return defaultGoto;
    }

    @Override
    public HashMap<Integer, Integer> get1() {
        return jumpOffsets;
    }
}