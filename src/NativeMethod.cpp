#include "NativeMethod.h"
#include "Frame.h"
#include <iostream>
#include "JavaHeap.h"
#include <string>

JType * ydk_lang_IO_print_str(RuntimeEnv * env){
    JObject * str = (JObject*)env->frames.top()->locals[0];
    auto & fields = env->jheap->getObject(str);
    JArray * chararr = (JArray*)fields[0];
    auto & lengthAndData = env->jheap->getArray(chararr);
    char * s = new char[lengthAndData.first + 1];
    for (int i = 0; i < lengthAndData.first; i++) {
        s[i] = (char)((JInt*)lengthAndData.second[i])->val;
    }
    s[lengthAndData.first] = '\0';
    std::cout << s;
    delete s;

    return nullptr;
}

JType * ydk_lang_IO_print_I(RuntimeEnv * env) {
    JInt * num = (JInt*)env->frames.top()->locals[0];
    std::cout << num->val;
    return nullptr;
}

JType * ydk_lang_IO_print_C(RuntimeEnv * env) {
    JInt * num = (JInt*)env->frames.top()->locals[0];
    std::cout << (char)num->val;
    return nullptr;
}

JType* java_lang_string_append_I(RuntimeEnv* env) {
    JObject * instance = dynamic_cast<JObject*>(env->frames.top()->locals[0]);
    JInt * numParameter = dynamic_cast<JInt*>(env->frames.top()->locals[1]);
    std::string str{};

    JArray * arr = dynamic_cast<JArray*>(env->jheap->getObjectField(*instance, 0));
    for(int i=0;i<arr->length;i++) {
        str += (char)dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
    }
    str += std::to_string(numParameter->val);
    JArray * newArr = env->jheap->createCharArray(str.c_str(), str.length());
    env->jheap->putObjectField(*instance, 0, newArr);

    env->jheap->removeArray(arr);
    return nullptr;
}

JType* java_lang_string_append_C(RuntimeEnv* env) {
    JObject * instance = dynamic_cast<JObject*>(env->frames.top()->locals[0]);
    JInt * numParameter = dynamic_cast<JInt*>(env->frames.top()->locals[1]);
    std::string str{};

    JArray * arr = dynamic_cast<JArray*>(env->jheap->getObjectField(*instance, 0));
    for (int i = 0; i<arr->length; i++) {
        str += (char)dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
    }
    char c = numParameter->val;
    str += c;
    JArray * newArr = env->jheap->createCharArray(str.c_str(), str.length());
    env->jheap->putObjectField(*instance, 0, newArr);

    env->jheap->removeArray(arr);
    return nullptr;
}

JType* java_lang_string_append_str(RuntimeEnv* env) {
    JObject * instance = dynamic_cast<JObject*>(env->frames.top()->locals[0]);
    JObject * strParameter = dynamic_cast<JObject*>(env->frames.top()->locals[1]);
    std::string str{};

    JArray * arr = dynamic_cast<JArray*>(env->jheap->getObjectField(*instance, 0));
    for (int i = 0; i<arr->length; i++) {
        str += (char)dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
    }
    JArray * chararr = dynamic_cast<JArray*>(env->jheap->getObjectField(*strParameter, 0));
    for (int i = 0; i < chararr->length; i++) {
        str += (char)dynamic_cast<JInt*>(env->jheap->getArrayItem(*chararr, i))->val;
    }

    JArray * newArr = env->jheap->createCharArray(str.c_str(), str.length());
    env->jheap->putObjectField(*instance, 0, newArr);

    env->jheap->removeArray(arr);
    return nullptr; 
}
