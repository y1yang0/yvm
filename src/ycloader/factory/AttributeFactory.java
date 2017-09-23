package ycloader.factory;

import ycloader.ClassFileReader;
import ycloader.adt.attribute.*;
import ycloader.adt.attribute.stackmaptable.StackMapTableAttribute;

public class AttributeFactory {
    public static Attribute create(ClassFileReader reader, String attributeName) {
        switch (attributeName) {
            case "ConstantValue":
                return new ConstantValueAttribute(reader);
            case "Code":
                return new CodeAttribute(reader);
            case "StackMapTable":
                return new StackMapTableAttribute(reader);
            case "Exceptions":
                return new ExceptionAttribute(reader);
            case "InnerClasses":
                return new InnerClassAttribute(reader);
            case "EnclosingMethod":
                return new EnclosingMethodAttribute(reader);
            case "Synthetic":
                return new SyntheticAttribute(reader);
            case "Signature":
                return new SignatureAttribute(reader);
            case "SourceFile":
                return new SourceFileAttribute(reader);
            case "SourceDebugExtension":
                return new SourceDebugExtensionAttribute(reader);
            case "LineNumberTable":
                return new LineNumberTableAttribute(reader);
            case "LocalVariableTable":
                return new LocalVariableTableAttribute(reader);
            case "LocalVariableTypeTable":
                return new LocalVariableTypeTableAttribute(reader);
            case "Deprecated":
                return new DeprecatedAttribute(reader);
            case "RuntimeVisibleAnnotations":
                return new RuntimeVisibleAnnotationsAttribute(reader);
            case "RuntimeInvisibleAnnotations":
                return new RuntimeInvisibleAnnotationsAttribute(reader);
            case "RuntimeVisibleParameterAnnotations":
                return new RuntimeVisibleParameterAnnotationsAttribute(reader);
            case "RuntimeInvisibleParameterAnnotations":
                return new RuntimeInvisibleParameterAnnotationsAttribute(reader);
            case "RuntimeVisibleTypeAnnotations":
                return new RuntimeVisibleTypeAnnotationsAttribute(reader);
            case "RuntimeInvisibleTypeAnnotations":
                return new RuntimeInvisibleTypeAnnotationsAttribute(reader);
            case "AnnotationDefault":
                return new AnnotationDefaultAttribute(reader);
            case "BootstrapMethods":
                return new BootstrapMethodsAttribute(reader);
            case "MethodParameters":
                return new MethodParametersAttribute(reader);
            default:
                return null;
        }
    }
}