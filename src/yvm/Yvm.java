package yvm;

import rtstruct.YThread;
import rtstruct.meta.MetaClass;
import ycloader.YClassLoader;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import yvm.adt.auxil.Tuple6;

public final class Yvm {
    public static void main(String[] args) {
        YThread thread = new YThread("test");
        thread.runTask(() -> {
            try {
                YClassLoader loader = new YClassLoader("java/io/File");
                Tuple6 bundle = loader.loadClass();
                MetaClass meta = loader.linkClass(bundle);
                loader.initializeClass(thread, meta);

            } catch (ClassLinkingException | ClassLoadingException | ClassInitializingException e) {
                e.printStackTrace();
            }
        });
    }
}
