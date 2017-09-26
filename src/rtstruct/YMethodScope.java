package rtstruct;

import rtstruct.meta.MetaClass;

import java.util.ArrayList;
import java.util.HashSet;

public class YMethodScope {
    private ArrayList<MetaClass> metas;
    private HashSet<String> namePool;

    public YMethodScope() {
        metas = new ArrayList<>();
        namePool = new HashSet<>();
    }

    public boolean addMetaClass(MetaClass metaClass) {
        if (metas.add(metaClass)) {
            namePool.add(metaClass.getQualifiedClassName());
            return true;
        }
        return false;
    }

    public MetaClass getMetaClass(String className, Class classLoader) {
        for (MetaClass m : metas) {
            if (m.getClassLoader() == classLoader && m.getQualifiedClassName().equals(className)) {
                return m;
            }
        }
        return null;
    }

    public boolean existClass(String className) {
        return namePool.contains(className);
    }
}
