package java.lang;

public class Thread {
    private Runnable task;

    public Thread(Runnable runnable) {
        this.task = runnable;
    }

    public synchronized native void start();
}
