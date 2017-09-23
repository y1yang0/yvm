package rtstruct;

import rtstruct.ystack.YStack;

public class YThread {
    private Thread thread;
    private int programCounter = -1;
    private YStack stack;
    private String threadName;

    public YThread(String threadName) {
        this.threadName = threadName;
    }

    public void runTask(Runnable task) {
        thread = new Thread(task);
        thread.setName(threadName);
        thread.start();
    }

    public void initiateStack() {
        stack = new YStack();
    }

    public YStack getStack() {
        return stack;
    }
}
