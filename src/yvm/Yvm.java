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

    private void loadInheritChain(YThread thread, YClassLoader loader, String leaves) throws ClassLoadingException, ClassLinkingException, ClassInitializingException {
        if (!methodScope.existClass(leaves)) {
            loader.associateThread(thread);
            Tuple6 bundle = loader.loadClass(leaves);
            MetaClass meta = loader.linkClass(bundle);
            methodScope.addMetaClass(meta);
            if (!meta.superClassName.isEmpty() && !methodScope.existClass(meta.superClassName)) {
                loadInheritChain(thread, loader, meta.superClassName);
                loader.initializeClass(meta);
            } else {
                //if there were loaded by the different class loader, then loading it again by this loader
                if (!meta.superClassName.isEmpty() && Predicate.isNull(methodScope.getMetaClass(meta.superClassName, loader.getClass()))) {
                    loadInheritChain(thread, loader, meta.superClassName);
                    loader.initializeClass(meta);
                }
            }
        } else {
            //if exist, check if they were loaded by the same class loader
            if (!leaves.isEmpty() && Predicate.isNull(methodScope.getMetaClass(leaves, loader.getClass()))) {
                loader.associateThread(thread);
                Tuple6 bundle = loader.loadClass(leaves);
                MetaClass meta = loader.linkClass(bundle);
                methodScope.addMetaClass(meta);

                if (!meta.superClassName.isEmpty() && !methodScope.existClass(meta.superClassName)) {
                    loadInheritChain(thread, loader, meta.superClassName);
                    loader.initializeClass(meta);
                } else {
                    //if there were loaded by the different class loader, then loading it again by this loader
                    if (!meta.superClassName.isEmpty() && Predicate.isNull(methodScope.getMetaClass(meta.superClassName, loader.getClass()))) {
                        loadInheritChain(thread, loader, meta.superClassName);
                        loader.initializeClass(meta);
                    }
                }
            }
        }
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

                loadInheritChain(classLoadingThread, loader, meta.superClassName);

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
