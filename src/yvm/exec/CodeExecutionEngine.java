package yvm.exec;

import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import rtstruct.meta.MetaClassMethod;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import yvm.adt.auxil.Tuple6;

import java.util.ArrayList;

public class CodeExecutionEngine {
    private YThread thread;

    private MetaClassMethod method;

    public CodeExecutionEngine(YThread thread) {
        this.thread = thread;
    }

    public void ignite(MetaClass metaClassInfo) {
        this.method = metaClassInfo.getMethods();
    }

    public void executeCLinit() throws ClassInitializingException {
        Tuple6<String, String, u1[], MetaClassMethod.StackRequirement,
                MetaClassMethod.ExceptionTable[], ArrayList<Attribute>> clinit = method.findMethod("<clinit>");
        if (clinit == null) {
            throw new ClassInitializingException("can not find synthetic <clinit> method");
        }

        //todo:execute clint method to initialize current class;
    }
}
