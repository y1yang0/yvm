#pragma once

#include "JavaType.h"
#include "RuntimeEnv.h"


JType* ydk_lang_IO_print_str(RuntimeEnv* env);
JType * ydk_lang_IO_print_I(RuntimeEnv * env);
JType * ydk_lang_IO_print_C(RuntimeEnv * env);

JType* java_lang_string_append_I(RuntimeEnv* env);
JType* java_lang_string_append_C(RuntimeEnv* env);
JType* java_lang_string_append_str(RuntimeEnv* env);