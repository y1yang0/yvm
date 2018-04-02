#ifndef YVM_PARSEUTIL_H
#define YVM_PARSEUTIL_H

#include "JavaType.h"
#include <typeinfo>
#include "Type.h"
#include <string>

/**
 * \brief Utilities that widely used in all aspects
 */

#define FOR_EACH(iter,var) for(decltype(var) iter =0; iter < var;iter++)
#define SHOULD_NOT_REACH_HERE static_assert(true,"Program should not reach here, it's a vital logical error\n");


struct Converter {
    /**
    * \brief Convert java.lang.String object to a stl string
    */
    static std::string javastring2stdtring(JObject * objectref);
};

/**
 * \brief Utilities for code execution engine
 */
inline u1 consumeU1(const u1* code, u4& opidx) {
    const u1 byte = code[++opidx];
    return byte;
}
inline u2 consumeU2(const u1* code, u4& opidx) {
    const u1 indexbyte1 = code[++opidx];
    const u1 indexbyte2 = code[++opidx];
    const u2 index = (indexbyte1 << 8) | indexbyte2;
    return index;
}

inline u4 consumeU4(const u1* code, u4& opidx) {
    const u1 byte1 = code[++opidx];
    const u1 byte2 = code[++opidx];
    const u1 byte3 = code[++opidx];
    const u1 byte4 = code[++opidx];
    const u4 res = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | (byte4);
    return res;
}

JType* cloneValue(JType* value);
bool hasInheritanceRelationship(const JavaClass * source, const JavaClass * super);

#define IS_COMPUTATIONAL_TYPE_1(value) (typeid(*value)!=typeid(JDouble) && typeid(*value)!=typeid(JLong))
#define IS_COMPUTATIONAL_TYPE_2(value) (typeid(*value)==typeid(JDouble) || typeid(*value)==typeid(JLong))

#define IS_JINT(x) (typeid(*x)==typeid(JInt))
#define IS_JLong(x) (typeid(*x)==typeid(JLong))
#define IS_JDouble(x) (typeid(*x)==typeid(JDouble))
#define IS_JFloat(x) (typeid(*x)==typeid(JFloat))
#define IS_JObject(x) (typeid(*x)==typeid(JObject))
#define IS_JArray(x) (typeid(*x)==typeid(JArray))
/**
 *\brief  Utilities for parsing class file
 */
#define IS_ATTR_ConstantValue(PTR) if(strcmp(PTR,"ConstantValue")==0)
#define IS_ATTR_Code(PTR) if(strcmp(PTR,"Code")==0)
#define IS_ATTR_StackMapTable(PTR) if(strcmp(PTR,"StackMapTable")==0)
#define IS_ATTR_Exceptions(PTR) if(strcmp(PTR,"Exceptions")==0)
#define IS_ATTR_BootstrapMethods(PTR) if(strcmp(PTR,"BootstrapMethods")==0)
#define IS_ATTR_InnerClasses(PTR) if(strcmp(PTR,"InnerClasses")==0)
#define IS_ATTR_EnclosingMethod(PTR) if(strcmp(PTR,"EnclosingMethod")==0)
#define IS_ATTR_Synthetic(PTR) if(strcmp(PTR,"Synthetic")==0)
#define IS_ATTR_Signature(PTR) if(strcmp(PTR,"Signature")==0)
#define IS_ATTR_RuntimeVisibleAnnotations(PTR) if(strcmp(PTR,"RuntimeVisibleAnnotations")==0)
#define IS_ATTR_RuntimeInvisibleAnnotations(PTR) if(strcmp(PTR,"RuntimeInvisibleAnnotations")==0)
#define IS_ATTR_RuntimeVisibleParameterAnnotations(PTR) if(strcmp(PTR,"RuntimeVisibleParameterAnnotations")==0)
#define IS_ATTR_RuntimeInvisibleParameterAnnotations(PTR) if(strcmp(PTR,"RuntimeInvisibleParameterAnnotations")==0)
#define IS_ATTR_RuntimeVisibleTypeAnnotations(PTR) if(strcmp(PTR,"RuntimeVisibleTypeAnnotations")==0)
#define IS_ATTR_RuntimeInvisibleTypeAnnotations(PTR) if(strcmp(PTR,"RuntimeInvisibleTypeAnnotations")==0)
#define IS_ATTR_AnnotationDefault(PTR) if(strcmp(PTR,"AnnotationDefault")==0)
#define IS_ATTR_MethodParameters(PTR) if(strcmp(PTR,"MethodParameters")==0)
#define IS_ATTR_SourceFile(PTR) if(strcmp(PTR,"SourceFile")==0)
#define IS_ATTR_SourceDebugExtension(PTR) if(strcmp(PTR,"SourceDebugExtension")==0)
#define IS_ATTR_LineNumberTable(PTR) if(strcmp(PTR,"LineNumberTable")==0)
#define IS_ATTR_LocalVariableTable(PTR) if(strcmp(PTR,"LocalVariableTable")==0)
#define IS_ATTR_LocalVariableTypeTable(PTR) if(strcmp(PTR,"LocalVariableTypeTable")==0)
#define IS_ATTR_Deprecated(PTR) if(strcmp(PTR,"Deprecated")==0)

#define IS_STACKFRAME_same_frame(num) ((num) >= 0&& (num) <= 63)
#define IS_STACKFRAME_same_locals_1_stack_item_frame(num) ((num) >= 64&& (num) <= 127)
#define IS_STACKFRAME_same_locals_1_stack_item_frame_extended(num) ((num) == 247)
#define IS_STACKFRAME_chop_frame(num) ((num) >= 248&& (num) <= 250)
#define IS_STACKFRAME_same_frame_extended(num) ((num) == 251)
#define IS_STACKFRAME_append_frame(num) ((num) >= 252 && (num) <= 254)
#define IS_STACKFRAME_full_frame(num) ((num) == 255)

#endif //YVM_PARSEUTIL_H
