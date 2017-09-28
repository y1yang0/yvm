package rtstruct;

import rtstruct.meta.MetaClass;

import java.util.ArrayList;
import java.util.List;

public class YObject implements Initializable {
    private MetaClass metaClassReference;
    private List<Object> fields;

    public YObject(MetaClass relatedMetaClass) {
        fields = new ArrayList<>();
        metaClassReference = relatedMetaClass;
    }

    public MetaClass getMetaReference() {
        return metaClassReference;
    }

    @Override
    public void init() {
        for (int i = 0; i < metaClassReference.fields.getFields().size(); i++) {
            fields.add(null);
        }
    }

}
