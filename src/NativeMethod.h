#ifndef YVM_NATIVEMETHOD_H
#define YVM_NATIVEMETHOD_H

#include "JavaType.h"
#include "RuntimeEnv.h"

JType* ydk_lang_IO_print_str(RuntimeEnv* env);
JType* ydk_lang_IO_print_I(RuntimeEnv* env);
JType* ydk_lang_IO_print_C(RuntimeEnv* env);
JType* ydk_lang_Math_random(RuntimeEnv* env);

JType* java_lang_stringbuilder_append_I(RuntimeEnv* env);
JType* java_lang_stringbuilder_append_C(RuntimeEnv* env);
JType* java_lang_stringbuilder_append_str(RuntimeEnv* env);
JType* java_lang_stringbuilder_append_D(RuntimeEnv* env);
JType* java_lang_stringbuilder_tostring(RuntimeEnv* env);

JType* java_lang_thread_start(RuntimeEnv* env);
#endif
