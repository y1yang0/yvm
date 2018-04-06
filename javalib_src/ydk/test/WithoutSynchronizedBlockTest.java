package ydk.test;

import ydk.lang.IO;

public class WithoutSynchronizedBlockTest {
    static class Increase implements Runnable{
        @Override
        public void run() {
            for (int i = 0; i < 50; i++) {
                IO.print(i+"\n");
            }
        }
    }
    public static void main(String[] args){
        new Thread(new Increase()).start();
        for(int i=0;i<50;i++){
            IO.print("---------------------------------\n");
        }
    }
}
