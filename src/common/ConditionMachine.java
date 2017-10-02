package common;

import java.util.ArrayList;
import java.util.List;

public class ConditionMachine {
    private List<Boolean> trueCondition;
    private List<Boolean> falseCondition;
    private boolean result;

    public ConditionMachine() {
        trueCondition = new ArrayList<>();
        falseCondition = new ArrayList<>();
        result = false;
    }

    public ConditionMachine shouldTrue(boolean cond) {
        trueCondition.add(cond);
        return this;
    }

    public ConditionMachine shouldFalse(boolean cond) {
        falseCondition.add(cond);
        return this;
    }

    public ConditionMachine yield(Runnable trueRunnable, Runnable falseRunnable) {
        if (trueCondition.isEmpty() && falseCondition.isEmpty()) {
            result = false;
            falseRunnable.run();
            return this;
        } else if (trueCondition.isEmpty() && !falseCondition.isEmpty()) {
            result = !falseCondition.contains(true);
            falseCondition.clear();
            if (result) {
                trueRunnable.run();
            } else {
                falseRunnable.run();
            }
            return this;
        } else if (!trueCondition.isEmpty() && falseCondition.isEmpty()) {
            result = !trueCondition.contains(false);
            trueCondition.clear();
            if (result) {
                trueRunnable.run();
            } else {
                falseRunnable.run();
            }
            return this;
        } else {
            result = !(trueCondition.contains(false) || falseCondition.contains(true));
            trueCondition.clear();
            falseCondition.clear();
            if (result) {
                trueRunnable.run();
            } else {
                falseRunnable.run();
            }
            return this;
        }
    }

    @SuppressWarnings("unused")
    public boolean getState() {
        return result;
    }
}
