package common;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Supplier;

public class ConditionMachine {
    private List<Boolean> trueCondition;
    private List<Boolean> falseCondition;

    public ConditionMachine() {
        trueCondition = new ArrayList<>();
        falseCondition = new ArrayList<>();
    }

    public ConditionMachine shouldTrue(boolean cond) {
        trueCondition.add(cond);
        return this;
    }

    public ConditionMachine shouldTrue(Supplier<Boolean> supplier) {
        trueCondition.add(supplier.get());
        return this;
    }

    public ConditionMachine shouldFalse(boolean cond) {
        falseCondition.add(cond);
        return this;
    }

    public boolean yield() {
        if (trueCondition.isEmpty() && falseCondition.isEmpty()) {
            return false;
        } else if (trueCondition.isEmpty() && !falseCondition.isEmpty()) {
            boolean result = !falseCondition.contains(true);
            falseCondition.clear();
            return result;
        } else if (!trueCondition.isEmpty() && falseCondition.isEmpty()) {
            boolean result = !trueCondition.contains(false);
            trueCondition.clear();
            return result;
        } else {
            boolean result = !(trueCondition.contains(false) || falseCondition.contains(true));
            trueCondition.clear();
            falseCondition.clear();
            return result;
        }
    }
}
