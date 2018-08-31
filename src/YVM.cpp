#include "Debug.h"
#include "Descriptor.h"
#include "GC.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "MethodArea.h"
#include "NativeMethod.h"
#include "Option.h"
#include "RuntimeEnv.h"
#include "Utils.h"
#include "YVM.h"

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

// Load given class into jvm
bool YVM::loadClass(const std::string& name) {
    return yrt.ma->loadJavaClass(name);
}

// link given class into jvm. A linkage exception would be occurred if given
// class not existed before linking
bool YVM::linkClass(const std::string& name) {
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage
        // exception to denote it;
        throw std::runtime_error(
            "LinkageException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->linkJavaClass(name);
    return true;
}

// Initialize given class.
bool YVM::initClass(CodeExecution& exec, const std::string& name) {
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage
        // exception to denote it;
        throw std::runtime_error(
            "InitializationException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->initClassIfAbsent(exec, name);
    return true;
}

// Call java's "public static void main(String...)" method in the newly created
// main thread. It also responsible for releasing reousrces and terminating
// virtual machine after main method executing accomplished
void YVM::callMain(const std::string& name) {
    executor.initialize(1);

    std::future<void> mainFuture = executor.submit([=]() -> void {
#ifdef YVM_DEBUG_SHOW_THREAD_NAME
        std::cout << "[Main Executing Thread] ID:" << std::this_thread::get_id()
                  << "\n";
#endif
        auto* jc = yrt.ma->loadClassIfAbsent(name);
        yrt.ma->linkClassIfAbsent(name);
        // For each execution thread, we have a code execution engine
        CodeExecution exec{};
        yrt.ma->initClassIfAbsent(exec, name);
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
    yrt.gc->terminateGC();

    // Terminate virtual machine normally
    return;
}

// Warm up yvm. This function would register native methods into jvm before
// actual code execution, and also initialize MethodArea with given java runtime
// paths, which is the core component of this jvm
void YVM::warmUp(const std::vector<std::string>& libPaths) {
    int p = sizeof nativeFunctionTable / sizeof nativeFunctionTable[0];
    for (int i = 0; i < p; i++) {
        registerNativeMethod(nativeFunctionTable[i][0],
                             nativeFunctionTable[i][1],
                             nativeFunctionTable[i][2],
                             reinterpret_cast<JType* (*)(RuntimeEnv*)>(
                                 const_cast<char*>(nativeFunctionTable[i][3])));
    }

    yrt.ma = new MethodArea(libPaths);
}
