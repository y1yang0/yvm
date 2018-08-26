#include "Frame.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "MethodArea.h"
#include "NativeMethod.h"
#include "YVM.h"
#include <future>
#include <iostream>
#include <random>
#include <string>

extern thread_local StackFrames frames;

JType* ydk_lang_IO_print_str(RuntimeEnv* env) {
	JObject* str        = (JObject*) frames.back()->locals[0];
	auto& fields        = env->jheap->getObjectFieldsByRef(str);
	JArray* chararr     = (JArray*) fields[0];
	auto& lengthAndData = env->jheap->getArrayItemsByRef(chararr);
	char* s             = new char[lengthAndData.first + 1];
	for (int i = 0; i < lengthAndData.first; i++) {
		s[i] = (char) ((JInt*) lengthAndData.second[i])->val;
	}
	s[lengthAndData.first] = '\0';
	std::cout << s;
	delete[] s;

	return nullptr;
}

JType* ydk_lang_IO_print_I(RuntimeEnv* env) {
	JInt* num = (JInt*) frames.back()->locals[0];
	std::cout << num->val;
	return nullptr;
}

JType* ydk_lang_IO_print_C(RuntimeEnv* env) {
	JInt* num = (JInt*) frames.back()->locals[0];
	std::cout << (char) num->val;
	return nullptr;
}

JType* ydk_lang_Math_random(RuntimeEnv* env) {
	std::default_random_engine dre;
	std::uniform_int_distribution<int> realD;
	return new JDouble(realD(dre));
}

JType* java_lang_stringbuilder_append_I(RuntimeEnv* env) {
	JObject* instance  = dynamic_cast<JObject*>(frames.back()->locals[0]);
	JInt* numParameter = dynamic_cast<JInt*>(frames.back()->locals[1]);
	std::string str{};

	JArray* arr = dynamic_cast<JArray*>(env->jheap->getObjectFieldByOffset(*instance, 0));
	if (arr != nullptr) {
		for (int i = 0; i < arr->length; i++) {
			str += (char) dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
		}
	}

	str += std::to_string(numParameter->val);
	JArray* newArr = env->jheap->createCharArray(str.c_str(), str.length());
	env->jheap->putObjectFieldByOffset(*instance, 0, newArr);

	env->jheap->removeArrayByRef(arr);
	return instance;
}

JType* java_lang_stringbuilder_append_C(RuntimeEnv* env) {
	JObject* instance  = dynamic_cast<JObject*>(frames.back()->locals[0]);
	JInt* numParameter = dynamic_cast<JInt*>(frames.back()->locals[1]);
	std::string str{};

	JArray* arr = dynamic_cast<JArray*>(env->jheap->getObjectFieldByOffset(*instance, 0));
	if (arr != nullptr) {
		for (int i = 0; i < arr->length; i++) {
			str += (char) dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
		}
	}
	char c = numParameter->val;
	str += c;
	JArray* newArr = env->jheap->createCharArray(str.c_str(), str.length());
	env->jheap->putObjectFieldByOffset(*instance, 0, newArr);

	env->jheap->removeArrayByRef(arr);
	return instance;
}

JType* java_lang_stringbuilder_append_str(RuntimeEnv* env) {
	JObject* instance     = dynamic_cast<JObject*>(frames.back()->locals[0]);
	JObject* strParameter = dynamic_cast<JObject*>(frames.back()->locals[1]);
	std::string str{};

	JArray* arr = dynamic_cast<JArray*>(env->jheap->getObjectFieldByOffset(*instance, 0));
	if (nullptr != arr) {
		for (int i = 0; i < arr->length; i++) {
			str += (char) dynamic_cast<JInt*>(env->jheap->getArrayItem(*arr, i))->val;
		}
	}
	JArray* chararr = dynamic_cast<JArray*>(env->jheap->getObjectFieldByOffset(*strParameter, 0));
	for (int i = 0; i < chararr->length; i++) {
		str += (char) dynamic_cast<JInt*>(env->jheap->getArrayItem(*chararr, i))->val;
	}

	JArray* newArr = env->jheap->createCharArray(str.c_str(), str.length());
	env->jheap->putObjectFieldByOffset(*instance, 0, newArr);

	env->jheap->removeArrayByRef(arr);
	return instance;
}

JType* java_lang_stringbuilder_tostring(RuntimeEnv* env) {
	JObject* instance = dynamic_cast<JObject*>(frames.back()->locals[0]);
	JArray* value = dynamic_cast<JArray*>(env->jheap->getObjectFieldByOffset(*instance, 0));
	char* carr = new char[value->length];
	for (int i = 0; i < value->length; i++) {
		carr[i] =
		    (char) dynamic_cast<JInt*>(env->jheap->getArrayItem(*value, i))->val;
	}
	JObject* str = env->jheap->createObject(*env->ma->findJavaClass("java/lang/"
	                                                                "String"));
	env->jheap->putObjectFieldByOffset(*str, 0,
	                                   env->jheap->createCharArray(carr, value->length));
	delete[] carr;

	return str;
}

JType* java_lang_thread_start(RuntimeEnv* env) {
	auto* instance     = dynamic_cast<JObject*>(frames.back()->locals[0]);
	auto* runnableTask = (JObject*) cloneValue(
	    dynamic_cast<JObject*>(env->jheap->getObjectFieldByName(env->ma->findJavaClass("java/lang/Thread"), "task", "Ljava/lang/Runnable;", instance)));

	YVM::executor.createThread();
	future<void> subThreadF = YVM::executor.submit([=]() {
#ifdef YVM_DEBUG_SHOW_THREAD_NAME
		std::cout << "[New Java Thread] ID:" << std::this_thread::get_id() << "\n";
#endif
		const char* name = runnableTask->jc->getClassName();
		auto* jc         = yrt.ma->loadClassIfAbsent(name);
		yrt.ma->linkClassIfAbsent(name);
		// For each execution thread, we have a code execution engine
		auto* frame = new Frame;
		frame->stack.push_back(runnableTask);
		frames.push_back(frame);
		CodeExecution exec{frame};

		yrt.ma->initClassIfAbsent(exec, name);
		// Push object reference and since Runnable.run() has no parameter, so
		// we dont need to push arguments since Runnable.run() has no parameter

		exec.invokeInterface(jc, "run", "()V");
	});
	YVM::executor.storeTaskFuture(subThreadF.share());

	return nullptr;
}
