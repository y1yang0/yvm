package runtime.meta;

import ycloader.adt.u2;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.dataobject.InterfacesObject;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class MetaClassInterface implements Resolvable<InterfacesObject> {
    private Map<Integer, String> interfaceMap;

    public MetaClassInterface() {
        interfaceMap = new HashMap<>();
    }

    @Override
    public void resolve(InterfacesObject interfaces, ConstantPoolObject cp) {
        for (u2 i : interfaces.getInterfaces()) {
            int index = cp.at(i.getValue()).getTableIndex();
            String name = cp.at(index).toString();
            interfaceMap.put(index, name);
        }
    }

    public Collection<String> getInterfaceNames() {
        return interfaceMap.values();
    }
}
