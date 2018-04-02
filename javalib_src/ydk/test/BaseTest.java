package ydk.test;


import ydk.lang.IO;

public class BaseTest
{
    private String baseName = "base";
    public BaseTest()
    {
        callName();
    }
    public void callName()
    {
        IO.print(baseName);
        //System.out.println(baseName);
    }
    static class Sub extends BaseTest
    {
        private String baseName = "sub";
        public void callName()
        {
            IO.print(baseName);
            //System.out.println(baseName);
        }
    }
    public static void main(String[] args)
    {
        BaseTest b = new Sub();
    }
}