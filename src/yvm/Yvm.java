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
import yvm.auxil.Predicate;

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

    public void loadInheritChain(YThread thread, YClassLoader loader, String leaves) throws ClassLoadingException, ClassLinkingException {
        if (!methodScope.existClass(leaves)) {
            loader.associateThread(thread);
            Tuple6 bundle = loader.loadClass(leaves);
            MetaClass meta = loader.linkClass(bundle);
            methodScope.addMetaClass(meta);
            if (!methodScope.existClass(meta.superClassName)) {
                loadInheritChain(thread, loader, meta.superClassName);
            } else {
                //if there were loaded by the different class loader, then loading it again by this loader
                if (Predicate.isNull(methodScope.getMetaClass(meta.superClassName, loader.getClass()))) {
                    loadInheritChain(thread, loader, meta.superClassName);
                }
            }
        } else {
            //if exist, check if they were loaded by the same class loader
            if (Predicate.isNull(methodScope.getMetaClass(leaves, loader.getClass()))) {
                loadInheritChain(thread, loader, leaves);
            }
        }
    }


    public void start() {
        YThread thread = new YThread("test");
        thread.associateVMHeap(heap);

        thread.runTask(() -> {
            try {
                YClassLoader loader = new YClassLoader();
                loader.associateThread(thread);

                Tuple6 bundle = loader.loadClass("ycloader/example/Example");
                MetaClass meta = loader.linkClass(bundle);
                methodScope.addMetaClass(meta);

                loadInheritChain(thread, loader, meta.superClassName);

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
