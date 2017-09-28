package yvm.auxil;

import java.util.concurrent.locks.Lock;

public class Continuation {
    public static void ifNullThrowNullptrException(Object object){
        if(object==null){
            throw new NullPointerException("the object \""+ object.toString() + "\" is null");
        }
    }

    public static void ifSynchronizedUnlock(Lock lock, boolean isSynchronized) {
        if (isSynchronized) {
            lock.unlock();
        }
    }
}
