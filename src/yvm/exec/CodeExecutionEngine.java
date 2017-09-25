package yvm.exec;

import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import rtstruct.meta.MetaClassMethod;
import rtstruct.ystack.YStackFrame;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.u1;
import ycloader.exception.ClassInitializingException;
import yvm.adt.*;

import java.util.ArrayList;

import static java.util.Objects.isNull;
import static yvm.auxil.Common.strNotEqual;

public class CodeExecutionEngine {
    private YThread thread;
    private boolean ignited;
    private MetaClass metaClass;

    public CodeExecutionEngine(YThread thread) {
        this.thread = thread;
        ignited = false;
    }

    public void ignite(MetaClass metaClassInfo) {
        this.metaClass = metaClassInfo;
        ignited = true;
    }

    public void executeCLinit() throws ClassInitializingException {
        if (!ignited) {
            throw new ClassInitializingException("code execution engine is not ready");
        }

        Tuple6<String, String, u1[], MetaClassMethod.StackRequirement,
                MetaClassMethod.ExceptionTable[], ArrayList<Attribute>>
                clinit = metaClass.getMethods().findMethod("<clinit>");

        if (isNull(clinit) || strNotEqual(clinit.get1Placeholder(), "<clinit>")) {
            throw new ClassInitializingException("can not find synthetic <clinit> method");
        }

        int maxLocals = clinit.get4Placeholder().maxLocals;
        int maxStack = clinit.get4Placeholder().maxStack;

        YStackFrame frame = new YStackFrame();
        frame.allocateSize(maxStack, maxLocals);
        thread.stack().pushFrame(frame);

        Opcode op = new Opcode(clinit.get3Placeholder());
        op.codes2Opcodes();
        //op.debug();
        codeExecution(op);
    }

    private void codeExecution(Opcode op) throws ClassInitializingException {
        ArrayList<Tuple3<Integer, Integer, Operand>>
                opcodes = op.getOpcodes();

        for (int i = 0; i < opcodes.size(); i++) {
            Tuple3 cd = opcodes.get(i);
            int programCount = (Integer) cd.get1Placeholder();
            thread.pc(programCount);
            switch ((Integer) cd.get2Placeholder()) {
                case Mnemonic.aaload:

                default:
                    throw new ClassInitializingException("unknown opcode in execution sequence");
            }
        }
    }
}
