package rtstruct;

import rtstruct.meta.MetaClass;

import java.util.ArrayList;

public class YMethodScope {
    private ArrayList<MetaClass> metas;

    public YMethodScope() {
        metas = new ArrayList<>();
    }

    public boolean addMetaClass(MetaClass metaClass) {
        return metas.add(metaClass);
    }

    public MetaClass getMetaClass(String className, Class classLoader) {
        for (MetaClass m : metas) {
            if (m.getClassLoader() == classLoader && m.getQualifiedClassName().equals(className)) {
                return m;
            }
        }
        return null;
    }

    public boolean existClass(String className, Class classLoader) {
        for (MetaClass m : metas) {
            if (m.getClassLoader() == classLoader && m.getQualifiedClassName().equals(className)) {
                return true;
            }
        }
        return false;
    }
}
