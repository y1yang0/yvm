package runtime.ystack;

import runtime.YObject;

import java.util.ArrayList;
import java.util.Stack;

public class YStackFrame {
    private ArrayList<YObject> vars;
    private Stack<YObject> operands;

    public YStackFrame() {
        vars = new ArrayList<>();
        operands = new Stack<>();
    }

    public void allocateSize(int maxStack, int maxLocal) {
        vars.ensureCapacity(maxStack);
        operands.setSize(maxStack);
    }

    public YObject peekOperand() {
        return operands.peek();
    }

    public YObject popOperand() {
        return operands.pop();
    }

    public void pushOperand(YObject item) {
        operands.push(item);
    }

    public YObject getLocalVariable(int index) {
        return vars.get(index);
    }

    public void setLocalVariable(int index, YObject value) {
        vars.set(index, value);
    }

    public void clearOperand() {
        operands.clear();
    }

    public void clearLocalVar() {
        vars.clear();
    }
}