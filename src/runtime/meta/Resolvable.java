package runtime.meta;

import ycloader.dataobject.ConstantPoolObject;

public interface Resolvable<ResolveType> {
    void resolve(ResolveType r, ConstantPoolObject cp);
}
