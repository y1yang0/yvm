// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef YVM_NATIVEMETHOD_H
#define YVM_NATIVEMETHOD_H

#include "../runtime/JavaType.h"
#include "../runtime/RuntimeEnv.h"

JType* ydk_lang_IO_print_str(RuntimeEnv* env, JType** args, int numArgs);
JType* ydk_lang_IO_print_I(RuntimeEnv* env, JType** args, int numArgs);
JType* ydk_lang_IO_print_C(RuntimeEnv* env, JType** args, int numArgs);

JType* java_lang_Math_random(RuntimeEnv* env, JType** args, int numArgs);
JType* java_lang_stringbuilder_append_I(RuntimeEnv* env, JType** args, int numArgs);
JType* java_lang_stringbuilder_append_C(RuntimeEnv* env, JType** args, int numArgs);
JType* java_lang_stringbuilder_append_str(RuntimeEnv* env, JType** args, int numArgs);
JType* java_lang_stringbuilder_append_D(RuntimeEnv* env, JType** args, int numArgs);
JType* java_lang_stringbuilder_tostring(RuntimeEnv* env, JType** args, int numArgs);

JType* java_lang_thread_start(RuntimeEnv* env, JType** args, int numArgs);
#endif
