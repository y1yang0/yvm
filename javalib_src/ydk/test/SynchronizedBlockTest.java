package ydk.test;

import ydk.lang.IO;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SynchronizedBlockTest {
    private static int cnt = 0;
    private static int cnt1 = 0;
    private static final Object lock = new Object();

    static class SafeIncrease implements Runnable{
        @Override
        public void run() {
            synchronized (lock) {
                for (int i = 0; i < 500000; i++) {
                    cnt++;
                }
            }
        }
    }
    static class Increase implements Runnable{
        @Override
        public void run() {
            for (int i = 0; i < 5000000; i++) {
                cnt1++;
            }
        }
    }
    public static void main(String[] args){
        new Thread(new SafeIncrease()).start();
        synchronized (lock) {
            for (int i = 0; i < 5000000; i++) {
                cnt--;
            }
        }
        //IO.print("Within Synchronized block...\n");
        //IO.print(cnt);
        System.out.println("SY"+cnt);
        new Thread(new Increase()).start();
        for (int i = 0; i < 5000000; i++) {
            cnt1--;
        }
        //IO.print("Without Synchronized block...\n");
        //IO.print(cnt1);
        System.out.println(cnt1);
    }
}
