#include "YVM.h"
#include "MethodArea.h"
#include "JavaClass.h"
#include "Utils.h"
#include "JavaHeap.h"
#include "Descriptor.h"
#include "Option.h"
#include "Debug.h"
#include "NativeMethod.h"
#include "RuntimeEnv.h"
#include "GC.h"

YVM::ExecutorThreadPool YVM::executor;

#define FORCE(x) (reinterpret_cast<char*>(x))
/**
* \brief {class_name,method_name,descriptor_name,function_pointer}
*/
static const char*((nativeFunctionTable[])[4]) = {
    {"ydk/lang/IO", "print", "(Ljava/lang/String;)V", FORCE(ydk_lang_IO_print_str)},
    {"ydk/lang/IO", "print", "(I)V", FORCE(ydk_lang_IO_print_I)},
    {"ydk/lang/IO", "print", "(C)V", FORCE(ydk_lang_IO_print_C)},

    {"java/lang/Math", "random", "()D", FORCE(ydk_lang_Math_random)},
    {"java/lang/StringBuilder", "append", "(I)Ljava/lang/StringBuilder;", FORCE(java_lang_stringbuilder_append_I)},
    {"java/lang/StringBuilder", "append", "(C)Ljava/lang/StringBuilder;", FORCE(java_lang_stringbuilder_append_C)},
    {
        "java/lang/StringBuilder", "append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;",
        FORCE(java_lang_stringbuilder_append_str)
    },
    {"java/lang/StringBuilder", "toString", "()Ljava/lang/String;", FORCE(java_lang_stringbuilder_tostring)},
    {"java/lang/Thread", "start", "()V", FORCE(java_lang_thread_start)}
};

YVM::YVM() {
#ifdef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE
    Inspector::printSizeofInternalTypes();
#endif
}

bool YVM::loadClass(const char* name) {
    return yrt.ma->loadJavaClass(name);
}

bool YVM::linkClass(const char* name) {
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage exception to denote it;
        throw std::runtime_error("LinkageException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->linkJavaClass(name);
    return true;
}

bool YVM::initClass(CodeExecution& exec, const char* name) {
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage exception to denote it;
        throw std::runtime_error("InitializationException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->initClassIfAbsent(exec, name);
    return true;
}

void YVM::callMain(const char* name) {
    executor.createThread();

    std::future<void> mainFuture = executor.submit([=]()->void {
#ifdef YVM_DEBUG_SHOW_THREAD_NAME
        std::cout << "[Main Executing Thread] ID:" << std::this_thread::get_id() << "\n";
#endif
        auto* jc = yrt.ma->loadClassIfAbsent(name);
        yrt.ma->linkClassIfAbsent(name);
        // For each execution thread, we have a code execution engine
        CodeExecution exec{};
        yrt.ma->initClassIfAbsent(exec, name);
        exec.invokeByName(jc, "main", "([Ljava/lang/String;)V");
    });

    // Block untile all sub threads accomplished its task
    for (const auto& start : executor.getTaskFutures()) {
        start.get();
    }
    // Block until main thread accomplished;
    mainFuture.get();

    // Close garbage collection. This is optional since operation system would release all resources when process exited
    yrt.gc->terminateGC();

    // Terminate virtual machine normally
    return;
}

void YVM::warmUp(const std::vector<std::string> & libPaths) {
    int p = sizeof nativeFunctionTable / sizeof nativeFunctionTable[0];
    for (int i = 0; i < p; i++) {
        registerNativeMethod(
            nativeFunctionTable[i][0],
            nativeFunctionTable[i][1],
            nativeFunctionTable[i][2],
            reinterpret_cast<JType *(*)(RuntimeEnv*)>(
                const_cast<char*>(nativeFunctionTable[i][3])));
    }

    // initialize runtime environment's search paths
    yrt.ma = new MethodArea(libPaths);
}
