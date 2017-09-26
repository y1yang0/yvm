package ycloader.dataobject;

import ycloader.BytesReaderProxy;
import ycloader.ClassFileReader;
import ycloader.Stuffable;
import ycloader.adt.constantpool.AbstractConstantPool;
import ycloader.adt.constantpool.ConstantClassInfo;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.constant.ConstantPoolTags;
import ycloader.exception.ClassLoadingException;
import ycloader.factory.ConstantPoolSFactory;

import java.io.IOException;
import java.util.ArrayList;

public class ConstantPoolObject extends BytesReaderProxy implements Stuffable {
    private ArrayList<AbstractConstantPool> pool = new ArrayList<>();
    private u2 poolLength;

    public ConstantPoolObject(ClassFileReader reader, u2 poolLength) {
        super(reader);
        this.poolLength = poolLength;
    }

    public ArrayList<AbstractConstantPool> getAbstractConstantPool() {
        return pool;
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        for (int i = 1; i <= poolLength.getValue() - 1; i++) {
            u1 tag = read1Byte();
            AbstractConstantPool info = ConstantPoolSFactory.create(getReader(), tag.getValue());
            if (info == null) {
                throw new ClassLoadingException("tag " + tag.getValue() + "does't existed");
            }
            info.setTableIndex(i);
            info.stuffing();
            pool.add(info);

            if (tag.getValue() == ConstantPoolTags.CONSTANT_Long || tag.getValue() == ConstantPoolTags.CONSTANT_Double) {
                i++;
            }
        }
    }

    public AbstractConstantPool at(int index) {
        for (AbstractConstantPool cb : pool) {
            if (index == cb.getTableIndex()) {
                return cb;
            }
        }
        return null;
    }

    public String getClassName(int index) {
        AbstractConstantPool classInfo = this.at(index);
        if (classInfo == null || classInfo.getClass() != ConstantClassInfo.class) {
            return "java/lang/Object";
        }
        return this.at(((ConstantClassInfo) classInfo).nameIndex.getValue()).toString();
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        for (AbstractConstantPool aPool : pool) {
            sb.append("{index:").append(aPool.getTableIndex()).append(" struct:");
            sb.append(aPool);
            sb.append("},");
        }
        sb.delete(sb.length() - 1, sb.length());
        sb.append("]");
        return sb.toString();
    }
}
