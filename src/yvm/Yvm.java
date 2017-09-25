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
        YThread thread = new YThread("test");
        thread.associateVMHeap(heap);

        thread.runTask(() -> {
            try {
                YClassLoader loader = new YClassLoader("ycloader/example/Example");
                loader.associateThread(thread);

                Tuple6 bundle = loader.loadClass();
                MetaClass meta = loader.linkClass(bundle);
                methodScope.addMetaClass(meta);

                loader.initializeClass(meta);
            } catch (ClassLinkingException | ClassLoadingException | ClassInitializingException e) {
                e.printStackTrace();
            }
        });
    }

    public synchronized YHeap getVMHeap() {
        return heap;
    }
}
