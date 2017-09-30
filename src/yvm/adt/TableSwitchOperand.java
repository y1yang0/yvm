package yvm.adt;

import java.util.HashMap;

public class TableSwitchOperand implements Operand<Integer, HashMap<Integer, Integer>, Integer, Integer> {
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

    @Override
    public Integer get2() {
        return null;
    }

    @Override
    public Integer get3() {
        return null;
    }
}
