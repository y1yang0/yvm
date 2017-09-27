package rtstruct.ystack;

import java.util.ArrayList;
import java.util.Stack;

class DynamicLinking {
}

class OperandStack extends Stack {
}

class LocalVarTable extends ArrayList<Object> {
}

public class YStackFrame {
    private LocalVarTable vars;
    private OperandStack operands;
    private DynamicLinking linker;

    public YStackFrame() {
        vars = new LocalVarTable();
        operands = new OperandStack();
        linker = new DynamicLinking();
    }

    public void allocateSize(int maxStack, int maxLocal) {
        vars.ensureCapacity(maxStack);
        operands.setSize(maxStack);
    }

    public Object peekOperand() {
        return operands.peek();
    }

    public Object pop$operand() {
        return operands.pop();
    }

    public <T> void push$operand(T item) {
        operands.push(item);
    }

    public Object getLocalVariable(int index) {
        return vars.get(index);
    }

    public void setLocalVariable(int index, Object value) {
        vars.set(index, value);
    }
}