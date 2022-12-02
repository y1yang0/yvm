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

#include "YVM.h"

#include "../gc/GC.h"
#include "../misc/Debug.h"
#include "../misc/NativeMethod.h"
#include "../misc/Option.h"
#include "../misc/Utils.h"
#include "../runtime/ClassSpace.h"
#include "../runtime/JavaClass.h"
#include "../runtime/JavaHeap.hpp"
#include "../runtime/RuntimeEnv.h"

YVM::ExecutorThreadPool YVM::executor;

#define FORCE(x) (reinterpret_cast<char*>(x))

// registered java native methods table, it conforms to following rule:
// {class_name,method_name,descriptor_name,function_pointer}
static const char*((nativeFunctionTable[])[4]) = {
    {"ydk/lang/IO", "print", "(Ljava/lang/String;)V",
     FORCE(ydk_lang_IO_print_str)},
    {"ydk/lang/IO", "print", "(I)V", FORCE(ydk_lang_IO_print_I)},
    {"ydk/lang/IO", "print", "(C)V", FORCE(ydk_lang_IO_print_C)},

    {"java/lang/Math", "random", "()D", FORCE(java_lang_Math_random)},
    {"java/lang/StringBuilder", "append", "(I)Ljava/lang/StringBuilder;",
     FORCE(java_lang_stringbuilder_append_I)},
    {"java/lang/StringBuilder", "append", "(C)Ljava/lang/StringBuilder;",
     FORCE(java_lang_stringbuilder_append_C)},
    {"java/lang/StringBuilder", "append", "(D)Ljava/lang/StringBuilder;",
     FORCE(java_lang_stringbuilder_append_D)},
    {"java/lang/StringBuilder", "append",
     "(Ljava/lang/String;)Ljava/lang/StringBuilder;",
     FORCE(java_lang_stringbuilder_append_str)},
    {"java/lang/StringBuilder", "toString", "()Ljava/lang/String;",
     FORCE(java_lang_stringbuilder_tostring)},
    {"java/lang/Thread", "start", "()V", FORCE(java_lang_thread_start)}

};

YVM::YVM() {
#ifdef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE
    Inspector::printSizeofInternalTypes();
#endif
}

// Call java's "public static void main(String...)" method in the newly created
// main thread. It is also responsible for releasing resources and terminating
// virtual machine after main method executing accomplished
void YVM::callMain(const std::string& name) {
    executor.initialize(1);

    std::future<void> mainFuture = executor.submit([=]() -> void {
#ifdef YVM_DEBUG_SHOW_THREAD_NAME
        std::cout << "[Main Executing Thread] ID:" << std::this_thread::get_id()
                  << "\n";
#endif
        auto* jc = runtime.cs->loadClassIfAbsent(name);
        runtime.cs->linkClassIfAbsent(name);
        // For each execution thread, we have a code execution engine
        Interpreter exec;
        runtime.cs->initClassIfAbsent(exec, name);
        exec.invokeByName(jc, "main", "([Ljava/lang/String;)V");
    });

    // Block until all sub threads accomplished its task
    for (const auto& start : executor.getTaskFutures()) {
        start.get();
    }
    // Block until main thread accomplished;
    mainFuture.get();

    // Close garbage collection. This is optional since operation system would
    // release all resources when process exited
    runtime.gc->terminateGC();
}

// Initialize yvm. This function would register native methods into jvm before
// actual code execution, and also initialize ClassSpace with given java runtime
// paths, which is the core component of this jvm
void YVM::initialize(const std::string& libPath) {
    int p = sizeof nativeFunctionTable / sizeof nativeFunctionTable[0];
    for (int i = 0; i < p; i++) {
        registerNativeMethod(
            nativeFunctionTable[i][0], nativeFunctionTable[i][1],
            nativeFunctionTable[i][2],
            reinterpret_cast<JType* (*)(RuntimeEnv*, JType**, int)>(
                const_cast<char*>(nativeFunctionTable[i][3])));
    }

    runtime.cs = new ClassSpace(libPath);
}
