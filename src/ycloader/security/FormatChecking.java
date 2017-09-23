package ycloader.security;

import ycloader.ClassFileReader;
import ycloader.adt.attribute.Attribute;
import ycloader.adt.attribute.BootstrapMethodsAttribute;
import ycloader.adt.constantpool.*;
import ycloader.adt.u4;
import ycloader.constant.MagicHex;
import ycloader.constant.ReferenceKind;
import ycloader.dataobject.ClassFileAttributeObject;
import ycloader.dataobject.ConstantPoolObject;
import ycloader.dataobject.FieldObject;
import ycloader.dataobject.MethodObject;

import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FormatChecking {
    public static class MagicNumber {
        public static boolean with(u4 magicNumber) {
            return magicNumber.getValue() == MagicHex.MAGIC_HEX;
        }
    }

    public static class ProperLengthOfAttribute {
        public static boolean with(FieldObject field, MethodObject method, ClassFileAttributeObject attribute) {
            for (Attribute fab : field.getAttributes()) {
                if (!fab.isValidAttribute()) {
                    return false;
                }
            }
            for (Attribute mab : method.getAttributes()) {
                if (!mab.isValidAttribute()) {
                    return false;
                }
            }
            for (Attribute aab : attribute.getAttributes()) {
                if (!aab.isValidAttribute()) {
                    return false;
                }
            }
            return true;
        }
    }

    public static class ClassFileNotTruncatedOrAdded {
        public static boolean with(ClassFileReader reader) {
            boolean res = false;
            try {
                res = reader.isEOF();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return res;
        }
    }

    public static class ConstantPoolConstraint {
        //ClassFileAttributeObject attrs for supporting CONSTANT_InvokeDynamicInfo checking;
        public static boolean with(ConstantPoolObject cp, ClassFileAttributeObject attrs) {
            for (AbstractConstantPool obj : cp.getConstantPoolObjects()) {
                if (obj instanceof ConstantClassInfo) {
                    int temp = ((ConstantClassInfo) obj).nameIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantUtf8Info.class) {
                        return false;
                    }
                } else if (obj instanceof ConstantFieldRefInfo) {
                    int temp = ((ConstantFieldRefInfo) obj).classIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantClassInfo.class) {
                        return false;
                    }
                    int temp1 = ((ConstantFieldRefInfo) obj).nameAndTypeIndex.getValue();
                    if (cp.at(temp1) == null || cp.at(temp1).getClass() != ConstantNameAndTypeInfo.class) {
                        return false;
                    }
                    //todo:check descriptor
                } else if (obj instanceof ConstantMethodRefInfo) {
                    int temp = ((ConstantMethodRefInfo) obj).classIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantClassInfo.class) {
                        return false;
                    }
                    int temp1 = ((ConstantMethodRefInfo) obj).nameAndTypeIndex.getValue();
                    if (cp.at(temp1) == null || cp.at(temp1).getClass() != ConstantNameAndTypeInfo.class) {
                        return false;
                    }
                    //todo:check descriptor
                } else if (obj instanceof ConstantInterfaceMethodRefInfo) {
                    int temp = ((ConstantInterfaceMethodRefInfo) obj).classIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantClassInfo.class) {
                        return false;
                    }
                    int temp1 = ((ConstantInterfaceMethodRefInfo) obj).nameAndTypeIndex.getValue();
                    if (cp.at(temp1) == null || cp.at(temp1).getClass() != ConstantNameAndTypeInfo.class) {
                        return false;
                    }
                    //todo:check descriptor
                } else if (obj instanceof ConstantStringInfo) {
                    int temp = ((ConstantStringInfo) obj).stringIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantUtf8Info.class) {
                        return false;
                    }
                } else if (obj instanceof ConstantIntegerInfo) {
                    u4 bs = ((ConstantIntegerInfo) obj).bytes;
                    if (bs.getValue() > Integer.MAX_VALUE || bs.getValue() < Integer.MIN_VALUE) {
                        return false;
                    }
                } else if (obj instanceof ConstantFloatInfo) {
                    //todo:check if it is a valid IEEE 754 floating-point single format
                } else if (obj instanceof ConstantLongInfo) {
                    int temp = obj.getTableIndex();
                    //check if it occupy two index items;
                    if (cp.at(temp + 1) != null) {
                        return false;
                    }
                    //todo:check if it is a valid IEEE 754 floating-point single format
                } else if (obj instanceof ConstantDoubleInfo) {
                    int temp = obj.getTableIndex();
                    //check if it occupy two index items;
                    if (cp.at(temp + 1) != null) {
                        return false;
                    }
                    //todo:check if it is a valid IEEE 754 floating-point single format
                } else if (obj instanceof ConstantNameAndTypeInfo) {
                    int temp = ((ConstantNameAndTypeInfo) obj).nameIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantUtf8Info.class) {
                        return false;
                    }
                    int temp1 = ((ConstantNameAndTypeInfo) obj).descriptorIndex.getValue();
                    if (cp.at(temp1) == null || cp.at(temp1).getClass() != ConstantUtf8Info.class) {
                        return false;

                    }
                } else if (obj instanceof ConstantUtf8Info) {
                    //todo:check if it is a utf8 string
                } else if (obj instanceof ConstantMethodHandleInfo) {
                    int refkind = ((ConstantMethodHandleInfo) obj).referenceKind.getValue();

                    if (refkind < 1 || refkind > 9) {
                        return false;
                    }
                    int refindex = ((ConstantMethodHandleInfo) obj).referenceIndex.getValue();
                    if (refkind == ReferenceKind.REF_getField || refkind == ReferenceKind.REF_getStatic ||
                            refkind == ReferenceKind.REF_putField || refkind == ReferenceKind.REF_putStatic) {
                        if (cp.at(refindex) == null || cp.at(refindex).getClass() != ConstantFieldRefInfo.class) {
                            return false;
                        }
                    } else if (refkind == ReferenceKind.REF_invokeVirtual || refkind == ReferenceKind.REF_newInvokeSpecial) {
                        if (cp.at(refindex) == null || cp.at(refindex).getClass() != ConstantMethodRefInfo.class) {
                            return false;
                        }
                    } else if (refkind == ReferenceKind.REF_invokeStatic || refkind == ReferenceKind.REF_invokeSpecial) {
                        if (cp.at(refindex) == null ||
                                (cp.at(refindex).getClass() != ConstantMethodRefInfo.class
                                        && cp.at(refindex).getClass() != ConstantInterfaceMethodRefInfo.class)) {
                            return false;
                        }
                    } else if (refkind == ReferenceKind.REF_invokeInterface) {
                        if (cp.at(refindex) == null || cp.at(refindex).getClass() != ConstantInterfaceMethodRefInfo.class) {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } else if (obj instanceof ConstantMethodTypeInfo) {
                    int temp = ((ConstantMethodTypeInfo) obj).descriptorIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantUtf8Info.class) {
                        return false;
                    }
                } else if (obj instanceof ConstantInvokeDynamicInfo) {
                    int temp = ((ConstantInvokeDynamicInfo) obj).nameAndTypeIndex.getValue();
                    if (cp.at(temp) == null || cp.at(temp).getClass() != ConstantNameAndTypeInfo.class) {
                        return false;
                    }
                    int bootstrapMethodAttrIndex = ((ConstantInvokeDynamicInfo) obj).bootstrapMethodAttrIndex.getValue();
                    for (Attribute ab : attrs.getClassFileAttributes()) {
                        if (ab instanceof BootstrapMethodsAttribute) {
                            if (bootstrapMethodAttrIndex >= ((BootstrapMethodsAttribute) ab).numBootstrapMethods.getValue()) {
                                return false;
                            }
                        }
                    }
                    return true;
                } else {
                    return false;
                }
            }
            return true;
        }
    }

    public static class FieldAndMethodValidation {
        private static boolean isValidName(String s) {
            return s.indexOf('.') == -1 || s.indexOf(';') == -1 || s.indexOf('[') == -1 || s.indexOf('/') == -1;
        }

        private static boolean isValidFieldDescriptor(String s) {
            Pattern pattern = Pattern.compile("(\\[)*[B|C|D|F|I|J|S|Z]*(L){0,1}(\\S)*");
            Matcher matcher = pattern.matcher(s);
            return matcher.matches();
        }

        private static boolean isValidMethodDescriptor(String s) {
            Pattern pattern = Pattern.compile("\\((\\[)*[B|C|D|F|I|J|S|Z]*(L){0,1}(\\S)*\\)[V|(\\[)*[B|C|D|F|I|J|S|Z]*(L){0,1}(\\S)*]*");
            Matcher matcher = pattern.matcher(s);
            return matcher.matches();
        }


        public static boolean with(FieldObject fields, MethodObject methods) {
            for (String name : fields.getNames()) {
                if (!isValidName(name)) {
                    return false;
                }
            }
            for (String descriptor : fields.getDescriptors()) {
                if (!isValidFieldDescriptor(descriptor)) {
                    return false;
                }
            }
            for (String methodName : methods.getNames()) {
                if (!isValidName(methodName)) {
                    return false;
                }
            }
            for (String methodDescriptor : methods.getDescriptors()) {
                if (!isValidMethodDescriptor(methodDescriptor)) {
                    return false;
                }
            }
            return true;
        }
    }
}
