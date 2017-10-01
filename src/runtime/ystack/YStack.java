package runtime.ystack;

import java.util.Stack;

public class YStack {
    private Stack<YStackFrame> stackFrames;

    public YStack() {
        stackFrames = new Stack<>();
    }

    public synchronized void pushFrame(YStackFrame frame) {
        stackFrames.push(frame);
    }

    public synchronized YStackFrame popFrame() {
        return stackFrames.pop();
    }

    public synchronized YStackFrame currentFrame() {
        return stackFrames.peek();
    }
}
