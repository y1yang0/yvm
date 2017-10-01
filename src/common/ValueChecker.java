package common;

import runtime.rtexception.VMExecutionException;

import java.lang.annotation.Annotation;
import java.lang.reflect.Field;

public class ValueChecker {
    private Class checkingClass;

    private ValueChecker(Class checkingClass) {
        this.checkingClass = checkingClass;
    }

    public static void safeCheck(Class checkingClass, Object object) throws IllegalAccessException {
        ValueChecker checker = new ValueChecker(checkingClass);
        checker.checkingNullValue(object);
    }

    public void checkingNullValue(Object object) throws IllegalAccessException {
        Field[] fields = checkingClass.getDeclaredFields();
        for (Field field : fields) {
            Annotation[] annotations = field.getDeclaredAnnotations();
            for (Annotation annotation : annotations) {
                if (annotation.annotationType() == ValueRequired.class) {
                    field.setAccessible(true);
                    if (field.get(object) == null) {
                        throw new VMExecutionException("the specified field" + field.getName() + " required a non-null value");
                    }
                }
            }
        }
    }
}
