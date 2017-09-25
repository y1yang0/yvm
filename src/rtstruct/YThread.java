package rtstruct;

import rtstruct.ystack.YStack;

/**
 *
 */
public class YThread {
    private Thread thread;
    private int programCounter = Integer.MIN_VALUE;
    private YStack stack;
    private String threadName;

    /**
     * @param threadName
     */
    public YThread(String threadName) {
        this.threadName = threadName;
        stack = new YStack();
    }

    /**
     * @param task
     */
    public void runTask(Runnable task) {
        thread = new Thread(task);
        thread.setName(threadName);
        thread.start();
    }

    /**
     * @return
     */
    public YStack stack() {
        return stack;
    }

    /**
     * get program counter if passing nothing
     *
     * @return
     */
    public int pc() {
        return programCounter;
    }

    /**
     * set program counter if passing a integer argument;
     * there it use the same "pc" as its method name for
     * simply code and in my thinking it's a very elegant way
     *
     * @param pc
     */
    public void pc(int pc) {
        this.programCounter = pc;
    }
}
