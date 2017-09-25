package rtstruct.ystack;

import java.util.Stack;

class DynamicLinking {
}

class ReturnAddress {
}

class OperandStack extends Stack {
}

class LocalVarTable extends Stack {
}

public class YStackFrame {
    private LocalVarTable vars;
    private OperandStack operands;
    private DynamicLinking linker;
    private ReturnAddress retAddr;

    public YStackFrame() {
        vars = new LocalVarTable();
        operands = new OperandStack();
        linker = new DynamicLinking();
        retAddr = new ReturnAddress();
    }

    public void allocateSize(int maxStack, int maxLocal) {
        vars.setSize(maxLocal);
        operands.setSize(maxStack);
    }

    public Object pop$operand() {
        return operands.pop();
    }

    public <T> void push$operand(T item) {
        operands.push(item);
    }
}