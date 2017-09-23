package ycloader.adt.attribute.stackmaptable;

import ycloader.ClassFileReader;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.ExcludeFields;
import ycloader.adt.u1;
import ycloader.adt.u2;
import ycloader.constant.VerificationTypeItems;
import ycloader.exception.ClassLoadingException;
import ycloader.factory.StackMapFrameFactory;
import ycloader.factory.VerificationTypeFactory;

import java.io.IOException;
import java.util.ArrayList;


/*
The StackMapTable attribute is a variable-length attribute in the attributes table
of a Code attribute (§4.7.3). A StackMapTable attribute is used during the process
of verification by type checking (§4.10.1).
There may be at most one StackMapTable attribute in the attributes table of a
Code attribute.
In a class file whose version number is 50.0 or above, if a method's Code attribute
does not have a StackMapTable attribute, it has an implicit stack map attribute
(§4.10.1). This implicit stack map attribute is equivalent to a StackMapTable
attribute with number_of_entries equal to zero.
 */

/**
 * A stack map frame specifies (either explicitly or implicitly) the bytecode offset at
 * which it applies, and the verification types of local variables and operand stack
 * entries for that offset.
 */


public class StackMapTableAttribute extends Attribute {
    private u2 numberOfEntries;
    private ArrayList<StackMapFrame> entries;

    public StackMapTableAttribute(ClassFileReader reader) {
        super(reader);
        entries = new ArrayList<>();
    }

    @Override
    public void stuffing() throws IOException, ClassLoadingException {
        attributeLength = read4Bytes();
        numberOfEntries = read2Bytes();

        for (int i = 0; i < numberOfEntries.getValue(); i++) {
            u1 frameType = read1Byte();

            StackMapFrame smf = StackMapFrameFactory.create(this, frameType.getValue());
            smf.stuffing();
            entries.add(smf);
        }
    }

    @Override
    @ExcludeFields
    public int getActualBytes() {
        int res = 2;
        for (StackMapFrame smf : entries) {
            res += smf.getActualBytes();
        }
        return res;
    }

    //public descriptor used for stack map frame factory in other package
    public class SMF_AppendFrame implements StackMapFrame {
        u1 frameType;    //range at [252,254]
        u2 offsetDelta;
        VerificationTypeInfo[] locals;   //length == frame_type - 251;


        public SMF_AppendFrame(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            offsetDelta = read2Bytes();
            locals = new VerificationTypeInfo[frameType.getValue() - 251];
            for (int i = 0; i < frameType.getValue() - 251; i++) {
                u1 temp = read1Byte();
                locals[i] = VerificationTypeFactory.create(StackMapTableAttribute.this, temp.getValue());
                locals[i].stuffing();
            }
        }

        @Override
        public int getActualBytes() {
            int res = 3;
            for (VerificationTypeInfo vti : locals) {
                res += vti.getActualBytes();
            }
            return res;
        }
    }

    public class SMF_ChopFrame implements StackMapFrame {
        u1 frameType;    //range at [248,250]
        u2 offsetDelta;

        public SMF_ChopFrame(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException {
            offsetDelta = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 3;
        }
    }

    public class SMF_FullFrame implements StackMapFrame {
        u1 frameType;    //range at 255
        u2 offsetDelta;
        u2 numberOfLocals;
        VerificationTypeInfo[] locals;   //length == numberOfLocals
        u2 numberOfStackItems;
        VerificationTypeInfo[] stack;    //length == numberOfStackItem

        public SMF_FullFrame(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            offsetDelta = read2Bytes();
            numberOfLocals = read2Bytes();

            locals = new VerificationTypeInfo[numberOfLocals.getValue()];

            for (int i = 0; i < numberOfLocals.getValue(); i++) {
                u1 temp = read1Byte();
                locals[i] = VerificationTypeFactory.create(StackMapTableAttribute.this, temp.getValue());
                locals[i].stuffing();
            }
            numberOfStackItems = read2Bytes();

            stack = new VerificationTypeInfo[numberOfStackItems.getValue()];
            for (int k = 0; k < numberOfStackItems.getValue(); k++) {
                u1 temp = read1Byte();
                VerificationTypeInfo vti = VerificationTypeFactory.create(StackMapTableAttribute.this, temp.getValue());
                vti.stuffing();
                stack[k] = vti;
            }
        }

        @Override
        public int getActualBytes() {
            int res = 7;
            for (VerificationTypeInfo vti1 : locals) {
                res += vti1.getActualBytes();
            }
            for (VerificationTypeInfo vti2 : stack) {
                res += vti2.getActualBytes();
            }
            return res;
        }
    }

    public class SMF_SameFrame implements StackMapFrame {
        u1 frameType;    //range at [0,63]

        public SMF_SameFrame(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException {
        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class SMF_SameFrameExtended implements StackMapFrame {
        u1 frameType;    //range at 251
        u2 offsetDelta;

        public SMF_SameFrameExtended(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException {
            offsetDelta = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 3;
        }
    }

    public class SMF_SameLocals1StackItemFrame implements StackMapFrame {
        u1 frameType;    //range at [64,127]
        VerificationTypeInfo vti;

        public SMF_SameLocals1StackItemFrame(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            u1 tag = read1Byte();
            vti = VerificationTypeFactory.create(StackMapTableAttribute.this, tag.getValue());
            vti.stuffing();
        }

        @Override
        public int getActualBytes() {
            return 1 + vti.getActualBytes();
        }
    }

    public class SMF_SameLocals1StackItemFrameExtended implements StackMapFrame {
        u1 frameType;    //range at 247
        u2 offsetDelta;
        VerificationTypeInfo vti;

        public SMF_SameLocals1StackItemFrameExtended(int n) {
            frameType = new u1(n);
        }

        @Override
        public void stuffing() throws IOException, ClassLoadingException {
            offsetDelta = read2Bytes();
            u1 tag = read1Byte();
            vti = VerificationTypeFactory.create(StackMapTableAttribute.this, tag.getValue());
            vti.stuffing();
        }

        @Override
        public int getActualBytes() {
            return 1 + 2 + vti.getActualBytes();
        }
    }

    public class VTI_DoubleVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Double);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_FloatVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Float);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_IntegerVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Integer);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_LongVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Long);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_NullVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Null);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_ObjectVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Object);
        u2 cpoolIndex;

        @Override
        public void stuffing() throws IOException {
            cpoolIndex = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 3;
        }
    }

    public class VTI_TopVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Top);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_UninitializedThisVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_UninitializedThis);

        @Override
        public void stuffing() throws IOException {

        }

        @Override
        public int getActualBytes() {
            return 1;
        }
    }

    public class VTI_UninitializedVariableInfo implements VerificationTypeInfo {
        public final u1 tag = new u1(VerificationTypeItems.ITEM_Uninitialized);
        u2 offset;

        @Override
        public void stuffing() throws IOException {
            offset = read2Bytes();
        }

        @Override
        public int getActualBytes() {
            return 3;
        }
    }
}

