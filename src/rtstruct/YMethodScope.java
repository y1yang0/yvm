package rtstruct;

import rtstruct.meta.MetaClass;

import java.util.HashSet;

public class YMethodScope {
    private HashSet<MetaClass> metas;

    public YMethodScope() {
        metas = new HashSet<>();
    }

    public boolean addMetaClass(MetaClass metaClass) {
        return metas.add(metaClass);
    }
}
