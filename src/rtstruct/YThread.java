package rtstruct;

import rtstruct.ystack.YStack;

class ProgramCounter {

}

public class YThread {
    private Thread thread = null;
    private ProgramCounter pc = null;
    private YStack stack = null;
    private String threadName = null;

    public YThread(String threadName) {
        this.threadName = threadName;
    }

    public void runTask(Runnable task) {
        thread = new Thread(task);
        thread.setName(threadName);
        thread.start();
    }

}
