#ifndef YVM_STATIC_H
#define YVM_STATIC_H

#define JAVA_9_MAJOR                            53
#define JAVA_8_MAJOR                            52
#define JAVA_7_MAJOR                            51
#define JAVA_6_MAJOR                            50

#define JAVA_CLASS_FILE_MAGIC_NUMBER            0XCAFEBABE

enum ConstantTag {
    TAG_Class =7,
    TAG_Fieldref =9,
    TAG_Methodref =10,
    TAG_InterfaceMethodref =11,
    TAG_String =8,
    TAG_Integer =3,
    TAG_Float= 4,
    TAG_Long= 5,
    TAG_Double =6,
    TAG_NameAndType =12,
    TAG_Utf8 =1,
    TAG_MethodHandle =15,
    TAG_MethodType =16,
    TAG_InvokeDynamic =18
};

enum VariableInfoTag {
    ITEM_Top = 0,
    ITEM_Integer = 1,
    ITEM_Float = 2,
    ITEM_Null = 5,
    ITEM_UninitializedThis = 6,
    ITEM_Object = 7,
    ITEM_Uninitialized = 8,
    ITEM_Long = 4,
    ITEM_Double = 3
};


#endif //YVM_STATIC_H
