package yvm;

import rtstruct.RuntimeVM;
import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import ycloader.YClassLoader;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.adt.Tuple6;

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

                Tuple6 bundle = loader.loadClass("java/math/BigInteger");
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
