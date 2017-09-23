package yvm.exec;

import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import rtstruct.meta.MetaClassMethod;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import yvm.adt.Tuple6;

import java.util.ArrayList;

import static java.util.Objects.isNull;

public class CodeExecutionEngine {
    private YThread thread;
    private boolean ignited;
    private MetaClassMethod method;

    public CodeExecutionEngine(YThread thread) {
        this.thread = thread;
        ignited = false;
    }

    public void ignite(MetaClass metaClassInfo) {
        this.method = metaClassInfo.getMethods();
        ignited = true;
    }

    public void executeCLinit() throws ClassInitializingException {
        if (!ignited) {
            throw new ClassInitializingException("code execution engine is not ready");
        }

        Tuple6<String, String, u1[], MetaClassMethod.StackRequirement,
                MetaClassMethod.ExceptionTable[], ArrayList<Attribute>>
                clinit = method.findMethod("<clinit>");

        if (isNull(clinit) || !clinit.get1Placeholder().equals("<clinit>")) {
            throw new ClassInitializingException("can not find synthetic <clinit> method");
        }

        int maxLocals = clinit.get4Placeholder().maxLocals;
        int maxStack = clinit.get4Placeholder().maxStack;

        //todo:execute clint method to initialize current class;
    }
}
