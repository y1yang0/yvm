package rtstruct;

public class RuntimeVM {
    private YHeap heap;
    private YMethodScope methodScope;

    public RuntimeVM(){
        heap = new YHeap();
        methodScope = new YMethodScope();
    }

    @SuppressWarnings("unused")
    public synchronized YHeap heap(){
        return heap;
    }

    public synchronized YMethodScope methodScope(){
        return methodScope;
    }
}
