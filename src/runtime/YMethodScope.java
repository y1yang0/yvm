package runtime;

import common.Predicate;
import runtime.meta.MetaClass;

import java.util.ArrayList;

public class YMethodScope {
    private ArrayList<MetaClass> metas;

    public YMethodScope() {
        metas = new ArrayList<>();
    }

    @SuppressWarnings("unused")
    public boolean addMetaClass(MetaClass metaClass) {
        return metas.add(metaClass);
    }

    public MetaClass getMetaClass(String className, Class classLoader) {
        for (MetaClass m : metas) {
            if (m.classLoader == classLoader && m.qualifiedClassName.equals(className)) {
                return m;
            }
        }
        return null;
    }

    public boolean existClass(String className, Class classLoader) {
        for (MetaClass m : metas) {
            if (m.classLoader == classLoader && m.qualifiedClassName.equals(className)) {
                return true;
            }
        }
        return false;
    }

    public boolean isSubclass(MetaClass a, MetaClass sub, Class classLoader) {
        if (!Predicate.isNull(sub.superClassName)) {
            if (sub.superClassName.equals(a.qualifiedClassName)
                    && getMetaClass(sub.superClassName, classLoader).classLoader == a.classLoader) {
                return true;
            } else {
                isSubclass(a, getMetaClass(sub.superClassName, classLoader), classLoader);
            }
        }
        return false;
    }

    public ArrayList<MetaClass> getAllClasses() {
        return metas;
    }
}
