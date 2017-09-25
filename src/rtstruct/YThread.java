package rtstruct;

import rtstruct.ystack.YStack;

public class YThread {
    private Thread thread;
    private int programCounter = Integer.MIN_VALUE;
    private YStack stack;
    private String threadName;

    private YHeap heapRef;

    public YThread(String threadName) {
        this.threadName = threadName;
        stack = new YStack();
    }

    public void runTask(Runnable task) {
        thread = new Thread(task);
        thread.setName(threadName);
        thread.start();
    }

    public void allocateVMHeap(YHeap heap) {
        heapRef = heap;
    }

    /**
     * get stack of related thread
     * @return
     */
    public YStack stack() {
        return stack;
    }

    /**
     * get0 program counter if passing nothing
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
