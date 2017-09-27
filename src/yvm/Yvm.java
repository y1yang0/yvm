package yvm;

import rtstruct.YHeap;
import rtstruct.YMethodScope;
import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import ycloader.YClassLoader;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.adt.Tuple6;

public final class Yvm {
    private YHeap heap;
    private YMethodScope methodScope;

    public Yvm() {
        heap = new YHeap();
        methodScope = new YMethodScope();
    }

    public static void main(String[] args) {
        Yvm vm = new Yvm();
        vm.start();
    }


    public void start() {
        YThread classLoadingThread = new YThread("classLoadingThread");
        classLoadingThread.associateVMHeap(heap);

        classLoadingThread.runTask(() -> {
            try {
                YClassLoader loader = new YClassLoader();
                loader.associateThread(classLoadingThread);

                Tuple6 bundle = loader.loadClass("ycloader/example/Example");
                MetaClass meta = loader.linkClass(bundle);
                methodScope.addMetaClass(meta);

                loader.loadInheritanceChain(methodScope, classLoadingThread, loader, meta.superClassName);

                loader.initializeClass(methodScope, meta);
            } catch (ClassLinkingException | ClassLoadingException | ClassInitializingException e) {
                e.printStackTrace();
            }
        });
    }

    @SuppressWarnings("unused")
    public synchronized YHeap getVMHeap() {
        return heap;
    }

    @SuppressWarnings("unused")
    public synchronized YMethodScope getMethodScope() {
        return methodScope;
    }
}
