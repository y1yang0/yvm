package yvm.adt;

import java.util.HashMap;

public class LookupSwitchOperand implements Operand<Integer, HashMap<Integer, Integer>, Integer, Integer> {
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

    @Override
    public Integer get2() {
        return null;
    }

    @Override
    public Integer get3() {
        return null;
    }
}
