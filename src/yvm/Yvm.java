package yvm;

import common.Tuple6;
import runtime.RuntimeVM;
import runtime.YThread;
import runtime.meta.MetaClass;
import ycloader.YClassLoader;
import ycloader.adt.u2;
import ycloader.dataobject.*;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;

public final class Yvm {
    private RuntimeVM runtimeVM;

    public Yvm() {
        runtimeVM = new RuntimeVM();
    }

    public static void main(String[] args) {
        Yvm vm = new Yvm();
        vm.start();
    }


    public void start() {
        YThread classLoadingThread = new YThread("classLoadingThread", runtimeVM);

        classLoadingThread.runTask(() -> {
            try {
                YClassLoader loader = new YClassLoader();
                loader.associateThread(classLoadingThread);

                Tuple6<ConstantPoolObject, InterfacesObject,
                        FieldObject, MethodObject,
                        ClassFileAttributeObject, u2[]>
                        bundle = loader.loadClass("java/math/BigInteger");
                MetaClass meta = loader.linkClass(bundle);
                runtimeVM.methodScope().addMetaClass(meta);
                loader.loadInheritanceChain(meta.superClassName);
                loader.initializeClass(meta);
            } catch (ClassLinkingException | ClassLoadingException | ClassInitializingException e) {
                e.printStackTrace();
            }
        });
    }

}
