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

#ifndef YVM_OPTION_H
#define YVM_OPTION_H

//--------------------------------------------------------------------------------
// denote the default threshold value of garbage collector. GC was started when
// the memory allocation was beyond this value.
//--------------------------------------------------------------------------------
#define YVM_GC_THRESHOLD_VALUE (1024 * 1024 * 10)

//--------------------------------------------------------------------------------
// show new spawning thread name
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_THREAD_NAME

//--------------------------------------------------------------------------------
// show byte size of internal structures
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE

//--------------------------------------------------------------------------------
// how executing bytecode and current class of method, method name and method
// descriptor
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_BYTECODE

//--------------------------------------------------------------------------------
// output *.class parsing result, which contains constant pool table, fields,
// methods, class access flag, interfaces, etc
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_CLASS_FILE
#ifdef YVM_DEBUG_CLASS_FILE
#define YVM_DEBUG_SHOW_VERSION
#define YVM_DEBUG_SHOW_CONSTANT_POOL_TABLE
#define YVM_DEBUG_SHOW_INTERFACE
#define YVM_DEBUG_SHOW_CLASS_FIELD
#define YVM_DEBUG_SHOW_CLASS_ACCESS_FLAGS
#define YVM_DEBUG_SHOW_CLASS_METHOD
#define YVM_DEBUG_SHOW_CLASS_ATTRIBUTE
#endif

//--------------------------------------------------------------------------------
// to mark a gc safe point
//--------------------------------------------------------------------------------
#define GC_SAFE_POINT

#endif  // !YVM_OPTION_H
