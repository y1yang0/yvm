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

#include "NativeMethod.h"

#include <future>
#include <iostream>
#include <random>
#include <string>

#include "../runtime/ClassSpace.h"
#include "../runtime/JavaClass.h"
#include "../runtime/JavaHeap.hpp"
#include "../vm/YVM.h"

JType* ydk_lang_IO_print_str(RuntimeEnv* env, JType** args, int numArgs) {
    JObject* str = (JObject*)args[0];
    if (nullptr != str) {
        auto fields = env->heap->getFields(str);
        JArray* chararr = (JArray*)fields[0];
        auto lengthAndData = env->heap->getElements(chararr);
        char* s = new char[lengthAndData.first + 1];
        for (int i = 0; i < lengthAndData.first; i++) {
            s[i] = (char)((JInt*)lengthAndData.second[i])->val;
        }
        s[lengthAndData.first] = '\0';
        std::cout << s;
        delete[] s;
    } else {
        std::cout << "null";
    }

    return nullptr;
}

JType* ydk_lang_IO_print_I(RuntimeEnv* env, JType** args, int numArgs) {
    JInt* num = (JInt*)args[0];
    std::cout << num->val;
    return nullptr;
}

JType* ydk_lang_IO_print_C(RuntimeEnv* env, JType** args, int numArgs) {
    JInt* num = (JInt*)args[0];
    std::cout << (char)num->val;
    return nullptr;
}

JType* java_lang_Math_random(RuntimeEnv* env, JType** args, int numArgs) {
    std::default_random_engine dre;
    std::uniform_int_distribution<int> realD;
    return new JDouble(realD(dre));
}

JType* java_lang_stringbuilder_append_I(RuntimeEnv* env, JType** args,
                                        int numArgs) {
    JObject* caller = (JObject*)args[0];
    JInt* numParameter = (JInt*)args[1];
    std::string str{};

    // append lhs string to str
    JArray* arr =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*caller, 0));
    if (arr != nullptr) {
        for (int i = 0; i < arr->length; i++) {
            str +=
                (char)dynamic_cast<JInt*>(env->heap->getElement(*arr, i))->val;
        }
    }

    // convert Int to string and append on str
    str += std::to_string(numParameter->val);
    JArray* newArr = env->heap->createCharArray(str, str.length());
    env->heap->putFieldByOffset(*caller, 0, newArr);

    // remove old lhs string since new str overlapped it
    if (arr != nullptr) {
        env->heap->removeArray(arr->offset);
    }

    return caller;
}

JType* java_lang_stringbuilder_append_C(RuntimeEnv* env, JType** args,
                                        int numArgs) {
    JObject* caller = (JObject*)args[0];
    JInt* numParameter = (JInt*)args[1];
    std::string str{};

    JArray* arr =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*caller, 0));
    if (arr != nullptr) {
        for (int i = 0; i < arr->length; i++) {
            str +=
                (char)dynamic_cast<JInt*>(env->heap->getElement(*arr, i))->val;
        }
    }
    char c = numParameter->val;
    str += c;
    JArray* newArr = env->heap->createCharArray(str, str.length());
    env->heap->putFieldByOffset(*caller, 0, newArr);
    if (arr != nullptr) {
        env->heap->removeArray(arr->offset);
    }
    return caller;
}

JType* java_lang_stringbuilder_append_str(RuntimeEnv* env, JType** args,
                                          int numArgs) {
    JObject* caller = (JObject*)args[0];
    JObject* strParameter = (JObject*)args[1];
    std::string str{};

    JArray* arr =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*caller, 0));
    if (nullptr != arr) {
        for (int i = 0; i < arr->length; i++) {
            str +=
                (char)dynamic_cast<JInt*>(env->heap->getElement(*arr, i))->val;
        }
    }
    JArray* chararr =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*strParameter, 0));
    for (int i = 0; i < chararr->length; i++) {
        str +=
            (char)dynamic_cast<JInt*>(env->heap->getElement(*chararr, i))->val;
    }

    JArray* newArr = env->heap->createCharArray(str, str.length());
    env->heap->putFieldByOffset(*caller, 0, newArr);

    if (arr != nullptr) {
        env->heap->removeArray(arr->offset);
    }
    return caller;
}

JType* java_lang_stringbuilder_append_D(RuntimeEnv* env, JType** args,
                                        int numArgs) {
    JObject* caller = (JObject*)args[0];
    JDouble* numParameter = (JDouble*)args[1];
    std::string str{};

    JArray* arr =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*caller, 0));
    if (arr != nullptr) {
        for (int i = 0; i < arr->length; i++) {
            str +=
                (char)dynamic_cast<JInt*>(env->heap->getElement(*arr, i))->val;
        }
    }
    str += std::to_string(numParameter->val);
    JArray* newArr = env->heap->createCharArray(str, str.length());
    env->heap->putFieldByOffset(*caller, 0, newArr);
    if (arr != nullptr) {
        env->heap->removeArray(arr->offset);
    }
    return caller;
}

JType* java_lang_stringbuilder_tostring(RuntimeEnv* env, JType** args,
                                        int numArgs) {
    JObject* caller = (JObject*)args[0];
    JArray* value =
        dynamic_cast<JArray*>(env->heap->getFieldByOffset(*caller, 0));
    char* carr = new char[value->length];
    for (int i = 0; i < value->length; i++) {
        carr[i] =
            (char)dynamic_cast<JInt*>(env->heap->getElement(*value, i))->val;
    }
    JObject* str =
        env->heap->createObject(*env->cs->findJavaClass("java/lang/String"));
    env->heap->putFieldByOffset(
        *str, 0, env->heap->createCharArray(carr, value->length));
    delete[] carr;

    return str;
}

JType* java_lang_thread_start(RuntimeEnv* env, JType** args, int numArgs) {
    auto* caller = (JObject*)args[0];
    auto* runnableTask = (JObject*)cloneValue(dynamic_cast<JObject*>(
        env->heap->getFieldByName(
            runtime.cs->findJavaClass("java/lang/Thread"),
                                   "task", "Ljava/lang/Runnable;", caller)));

    YVM::executor.createThread();
    future<void> subThreadF = YVM::executor.submit([=]() {
#ifdef YVM_DEBUG_SHOW_THREAD_NAME
        std::cout << "[New Java Thread] ID:" << std::this_thread::get_id()
                  << "\n";
#endif
        const std::string& name = runnableTask->jc->getClassName();
        auto* jc = runtime.cs->loadClassIfAbsent(name);
        runtime.cs->linkClassIfAbsent(name);
        // For each execution thread, we have a code execution engine
        auto* frame = new JavaFrame;
        frame->pushFrame(1, 1);
        frame->top()->push(runnableTask);
        Interpreter exec{frame};

        runtime.cs->initClassIfAbsent(exec, name);
        // Push object reference and since Runnable.run() has no parameter, so
        // we dont need to push arguments since Runnable.run() has no parameter

        exec.invokeInterface(jc, "run", "()V");
    });
    YVM::executor.storeTaskFuture(subThreadF.share());

    return nullptr;
}
