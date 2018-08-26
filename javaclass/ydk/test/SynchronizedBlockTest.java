package ydk.test;

import ydk.lang.IO;

public class SynchronizedBlockTest {
    private static int cnt = 0;
    private static int cnt2 = 0;
    private static final Object lock = new Object();

    static class SafeIncrease implements Runnable{
        @Override
        public void run() {
            synchronized (lock) {
                for (int i = 0; i < 50; i++) {
                    IO.print(i+"\n");
                }
            }
        }
    }
    public static void main(String[] args){
        new Thread(new SafeIncrease()).start();
        synchronized (lock){
            for(int i=0;i<50;i++){
                IO.print("---------------------------------\n");
            }
        }
    }
}
