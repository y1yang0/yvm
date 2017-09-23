package ycloader.factory;

import ycloader.adt.attribute.stackmaptable.StackMapTableAttribute;
import ycloader.adt.attribute.stackmaptable.VerificationTypeInfo;
import ycloader.constant.VerificationTypeItems;
import ycloader.exception.ClassLoadingException;

public class VerificationTypeFactory {
    public static VerificationTypeInfo create(StackMapTableAttribute outer, int n) throws ClassLoadingException {
        switch (n) {
            case VerificationTypeItems.ITEM_Top:
                return outer.new VTI_TopVariableInfo();
            case VerificationTypeItems.ITEM_Integer:
                return outer.new VTI_IntegerVariableInfo();
            case VerificationTypeItems.ITEM_Float:
                return outer.new VTI_FloatVariableInfo();
            case VerificationTypeItems.ITEM_Double:
                return outer.new VTI_DoubleVariableInfo();
            case VerificationTypeItems.ITEM_Long:
                return outer.new VTI_LongVariableInfo();
            case VerificationTypeItems.ITEM_Null:
                return outer.new VTI_NullVariableInfo();
            case VerificationTypeItems.ITEM_UninitializedThis:
                return outer.new VTI_UninitializedThisVariableInfo();
            case VerificationTypeItems.ITEM_Object:
                return outer.new VTI_ObjectVariableInfo();
            case VerificationTypeItems.ITEM_Uninitialized:
                return outer.new VTI_UninitializedVariableInfo();
            default:
                throw new ClassLoadingException("verification type out of range: tag " + n + " doesn't match any valid verification type");
        }
    }
}
