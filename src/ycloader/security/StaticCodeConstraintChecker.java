package ycloader.security;

import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.CodeAttribute;
import ycloader.dataobject.MethodObject;

import java.util.ArrayList;

class StaticCodeConstraintChecker {
    private MethodObject methods;
    private ArrayList<Attribute> attrs;

    public StaticCodeConstraintChecker(MethodObject methodObject) {
        methods = methodObject;
        attrs = null;
    }

    public void promiseCode() {
        attrs = methods.getAttributes();
        for (Attribute attr : attrs) {
            if (attr instanceof CodeAttribute) {
                CodeAttribute code = (CodeAttribute) attr;
            }
        }
    }
}
