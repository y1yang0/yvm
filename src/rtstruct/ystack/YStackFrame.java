package rtstruct.ystack;

class DynamicLinking {
}

class ReturnAddress {
}

class OperandStack {
}

class LocalVarTable {
}

public class YStackFrame {
    private int vars;
    private OperandStack operands = null;
    private DynamicLinking linker = null;
    private ReturnAddress retAddr = null;


}