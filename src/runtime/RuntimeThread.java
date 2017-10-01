package runtime;

import runtime.ystack.YStack;

public class RuntimeThread {
    private int programCounter = Integer.MIN_VALUE;
    private YStack stack;

    public RuntimeThread() {
        stack = new YStack();
    }

    public int pc() {
        return programCounter;
    }

    public void pc(int n) {
        programCounter = n;
    }

    public YStack stack() {
        return stack;
    }
}
