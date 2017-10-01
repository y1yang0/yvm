package yvm.auxil;

import runtime.YArray;

import java.util.concurrent.locks.Lock;

import static common.Predicate.inRange;

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

    public static void ifNotInThrowOutOfRangeException(YArray arrayRef, int index) {
        if (!inRange(arrayRef, index)) {
            throw new ArrayIndexOutOfBoundsException("array index " + index + " out of bounds");
        }
    }
}
