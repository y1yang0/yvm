package ycloader;

import rtstruct.YMethodScope;
import rtstruct.YThread;
import rtstruct.meta.*;
import ycloader.adt.u2;
import ycloader.adt.u4;
import ycloader.constant.ClassAccessProperty;
import ycloader.dataobject.*;
import ycloader.exception.ClassInitializingException;
import ycloader.exception.ClassLinkingException;
import ycloader.exception.ClassLoadingException;
import ycloader.security.FormatChecking;
import yvm.adt.Tuple6;
import yvm.auxil.Peel;
import yvm.exec.CodeExecutionEngine;

import java.io.EOFException;
import java.io.IOException;
import java.util.Collection;

/**
 * This YClassLoader(Yet another java Class file Loader) is based on
 * The Java® VirtualMachine Specification - Java SE 8 Edition which could
 * be download and read online at
 * http://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf
 * <p>
 * Parsing action simply followed the instruction and descriptions as
 * The Class Format Chapter in manual said;
 *
 * @author: racaljk
 * @email: 1948638989@qq.com
 */

public class YClassLoader {
    private ClassFileReader reader;

    private YThread threadRef;

    private String javaClass;

    @SuppressWarnings("unused")
    public YClassLoader(String javaClass) {
        this.javaClass = javaClass;
    }

    @SuppressWarnings("unused")
    public YClassLoader() {

    }

    @SuppressWarnings({"unchecked","unused"})
    public Tuple6<ConstantPoolObject, InterfacesObject,
            FieldObject, MethodObject,
            ClassFileAttributeObject, u2[]> loadClass(String javaClass) throws ClassLoadingException {

        this.javaClass = javaClass;

        setClassFileReader();

        try {
            if (!reader.openDataInputStream()) {
                throw new IOException("Failed to get " + reader.getCurrentHandlingClassName() + " class file data, check your config file in ./conf/rtsearch.xml");
            }

            if (!FormatChecking.MagicNumber.with(read4Bytes())) {
                throw new ClassFormatError("it is not a java dot class file");
            }
            u2 minorVersion = read2Bytes();
            u2 majorVersion = read2Bytes();

            u2 constantPoolCount = read2Bytes();
            ConstantPoolObject poolObj = new ConstantPoolObject(reader, constantPoolCount);
            poolObj.stuffing();

            u2 accessFlag = read2Bytes();
            u2 thisClass = read2Bytes();
            u2 superClass = read2Bytes();
            u2 interfaceCount = read2Bytes();

            InterfacesObject interfaceObj = new InterfacesObject(reader, interfaceCount.getValue());
            interfaceObj.stuffing();

            u2 fieldsCount = read2Bytes();
            FieldObject fieldObj = new FieldObject(reader, poolObj, fieldsCount.getValue());
            fieldObj.stuffing();

            u2 methodsCount = read2Bytes();
            MethodObject methodObj = new MethodObject(reader, poolObj, methodsCount.getValue());
            methodObj.stuffing();

            u2 classFileAttributeCount = read2Bytes();
            ClassFileAttributeObject attrObj = new ClassFileAttributeObject(reader, poolObj, classFileAttributeCount.getValue());
            attrObj.stuffing();

            if (!FormatChecking.ClassFileNotTruncatedOrAdded.with(reader)) {
                throw new ClassFormatError(".class file may be truncated or added extra bytes");
            }
            return new Tuple6<>(poolObj, interfaceObj, fieldObj, methodObj, attrObj, new u2[]{accessFlag, thisClass, superClass});
        } catch (EOFException e) {
            try {
                throw new IOException("unexpected EOF, class file format error");
            } catch (IOException e1) {
                e1.printStackTrace();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public MetaClass linkClass(Tuple6<ConstantPoolObject, InterfacesObject,
            FieldObject, MethodObject,
            ClassFileAttributeObject, u2[]> bundle) throws ClassLinkingException {
        verify(bundle);
        MetaClass m = resolve(bundle);
        return m;
    }

    public void initializeClass(MetaClass meta) throws ClassInitializingException {
        CodeExecutionEngine engine = new CodeExecutionEngine();
        engine.ignite(meta,YClassLoader.this);
        engine.executeMethod("<clinit>");
    }

    private void verify(Tuple6<ConstantPoolObject, InterfacesObject,
            FieldObject, MethodObject,
            ClassFileAttributeObject, u2[]> bundle) throws ClassLinkingException {
        if (!FormatChecking.ProperLengthOfAttribute.with(bundle.get3Placeholder(), bundle.get4Placeholder(), bundle.get5Placeholder())) {
            throw new ClassLinkingException("attributes in .class file doesn't have proper length");
        }

        if (!FormatChecking.ConstantPoolConstraint.with(bundle.get1Placeholder(), bundle.get5Placeholder())) {
            throw new ClassLinkingException("invalid constant pool index");
        }
        if (!FormatChecking.FieldAndMethodValidation.with(bundle.get3Placeholder(), bundle.get4Placeholder())) {
            throw new ClassLinkingException("invalid field|method name or descriptor");
        }
    }

    private MetaClass resolve(Tuple6<ConstantPoolObject, InterfacesObject,
            FieldObject, MethodObject,
            ClassFileAttributeObject, u2[]> bundle) throws ClassLinkingException {
        ConstantPoolObject cp = bundle.get1Placeholder();
        InterfacesObject inter = bundle.get2Placeholder();
        FieldObject field = bundle.get3Placeholder();
        MethodObject method = bundle.get4Placeholder();
        ClassFileAttributeObject attr = bundle.get5Placeholder();

        MetaClass meta = new MetaClass();
        meta.qualifiedClassName = cp.getClassName(bundle.get6Placeholder()[1].getValue());
        meta.classLoader = YClassLoader.class;

        meta.superClassName = cp.getClassName(bundle.get6Placeholder()[2].getValue());
        meta.accessFlag = bundle.get6Placeholder()[0].getValue();
        meta.isClass = (ClassAccessProperty.ACC_INTERFACE & meta.accessFlag) != 200;

        MetaClassInterface resolvedInterface = new MetaClassInterface();
        resolvedInterface.resolve(inter, cp);
        meta.interfaces = resolvedInterface;

        MetaClassField resolvedField = new MetaClassField(meta);
        resolvedField.resolve(field, cp);
        meta.fields = resolvedField;
        //resolvedField.debug();

        MetaClassMethod resolvedMethod = new MetaClassMethod();
        resolvedMethod.resolve(method, cp);
        meta.methods = resolvedMethod;
        //resolvedMethod.debug();

        MetaClassConstantPool resolvedConstantPool = new MetaClassConstantPool();
        resolvedConstantPool.resolve(cp);
        meta.constantPool = resolvedConstantPool;
        resolvedConstantPool.debug(cp.getClassName(bundle.get6Placeholder()[1].getValue()));
        return meta;
    }

    public void loadInheritanceChain(String leaves)
            throws ClassLoadingException, ClassLinkingException, ClassInitializingException {
        YMethodScope methodScope = threadRef.runtimeVM().methodScope();

        if (!methodScope.existClass(leaves, this.getClass())) {
            this.associateThread(threadRef);
            Tuple6 bundle = this.loadClass(leaves);
            MetaClass meta = this.linkClass(bundle);
            methodScope.addMetaClass(meta);

            if (!meta.superClassName.isEmpty() && !methodScope.existClass(meta.superClassName, this.getClass())) {
                loadInheritanceChain(meta.superClassName);
            }

            this.initializeClass(meta);
        }
    }

    @Deprecated
    public void loadRelatedClasses(MetaClass metaClass)
            throws ClassLoadingException, ClassLinkingException, ClassInitializingException {
        Collection<String> classes = metaClass.constantPool.getClassNames();
        for (String aClass : classes) {
            String peeledClass = Peel.peelFieldDescriptor(aClass).get(0);
            if (!threadRef.runtimeVM().methodScope().existClass(peeledClass, this.getClass())) {
                Tuple6 bundle = loadClass(peeledClass);
                MetaClass meta = linkClass(bundle);
                threadRef.runtimeVM().methodScope().addMetaClass(meta);
                loadInheritanceChain(meta.superClassName);
                initializeClass(meta);
            }
        }
    }

    private synchronized void setClassFileReader() throws ClassLoadingException {
        if (this.javaClass.isEmpty()) {
            throw new ClassLoadingException("no java class file to be loaded");
        }
        reader = new ClassFileReader(this.javaClass);
    }

    private synchronized u2 read2Bytes() throws IOException {
        return reader.read2Bytes();
    }

    private synchronized u4 read4Bytes() throws IOException {
        return reader.read4Bytes();
    }

    public synchronized void associateThread(YThread thread) {
        threadRef = thread;
    }

    public synchronized YThread getStartupThread(){
        return threadRef;
    }

}
