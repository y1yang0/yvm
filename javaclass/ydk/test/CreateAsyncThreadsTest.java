package ydk.test;

// synchronized支持对象(数组)锁,但不支持.class锁(因为现在还没有实现反射)

import ydk.lang.IO;
import java.lang.Runnable;
import java.lang.Thread;

public class CreateAsyncThreadsTest {
    private static int cnt = 0;
    static class Task implements Runnable {
        @Override
        public void run() {
            IO.print("This is ");
            for(int i=0;i<10;i++){
                cnt++;
            }
            IO.print(cnt);
            IO.print(" times to say \"Hello World\"\n");
        }
    }

    public static void main(String[] args){
        for(int i=0;i<1000;i++){
            new Thread(new Task()).start();
        }
    }
}
