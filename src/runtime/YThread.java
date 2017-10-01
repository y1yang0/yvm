package runtime;

public class YThread {
    private Thread thread;
    private String threadName;

    private RuntimeVM runtimeVMRef;
    private RuntimeThread runtimeThread;

    public YThread(String threadName, RuntimeVM runtimeVM) {
        this.threadName = threadName;
        this.runtimeVMRef = runtimeVM;
        runtimeThread = new RuntimeThread();
    }

    public void runTask(Runnable task) {
        thread = new Thread(task);
        thread.setName(threadName);
        thread.start();
    }

    /**
     * get runtime virtual machine data scope
     * @return
     */
    public RuntimeVM runtimeVM() {
        return runtimeVMRef;
    }

    public RuntimeThread runtimeThread() {
        return runtimeThread;
    }
}
