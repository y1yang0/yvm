package rtstruct.ystack;

import java.util.ArrayList;
import java.util.Stack;

public class YStackFrame {
    private LocalVarTable vars;
    private OperandStack operands;

    public YStackFrame() {
        vars = new LocalVarTable();
        operands = new OperandStack();
    }

    public void allocateSize(int maxStack, int maxLocal) {
        vars.ensureCapacity(maxStack);
        operands.setSize(maxStack);
    }

    public Object peekOperand() {
        return operands.peek();
    }

    public Object popOperand() {
        return operands.pop();
    }

    public void pushOperand(Object item) {
        operands.push(item);
    }

    public Object getLocalVariable(int index) {
        return vars.get(index);
    }

    public void setLocalVariable(int index, Object value) {
        vars.set(index, value);
    }

    class OperandStack extends Stack {
    }

    class LocalVarTable extends ArrayList<Object> {
    }
}