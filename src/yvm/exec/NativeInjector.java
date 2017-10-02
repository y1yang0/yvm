package yvm.exec;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class NativeInjector {
    private Method method;
    private Object caller;

    public NativeInjector load(String libraryName) {
        System.loadLibrary(libraryName);
        return this;
    }

    public NativeInjector getMethod(Class className, String methodName, Class<?>... parameterTypes) {
        try {
            caller = className.newInstance();
            method = className.getDeclaredMethod(methodName, parameterTypes);
        } catch (InstantiationException | IllegalAccessException | NoSuchMethodException e) {
            e.printStackTrace();
        }
        method.setAccessible(true);

        return this;
    }

    public void invoke(Object... args) {
        try {
            method.invoke(caller, args);
        } catch (IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
        }
    }
}
