#include "YVM.h"
#include <exception>
#include <fstream>
#include "MethodArea.h"
#include "JavaClass.h"
#include "Utils.h"
#include "AccessFlag.h"
#include "JavaHeap.h"
#include "Descriptor.h"
#include "Option.h"
#include "Debug.h"
#include "JavaClass.h"
#include "NativeMethod.h"
#include "RuntimeEnv.h"

#define FORCE(x) reinterpret_cast<char*>(x)
/**
* \brief {class_name,method_name,descriptor_name,function_pointer}
*/
static const char*((nativeFunctionTable[])[4]) = {
    { "ydk/lang/IO", "print", "(Ljava/lang/String;)V", FORCE(ydk_lang_IO_print_str) },
    { "ydk/lang/IO", "print", "(I)V", FORCE(ydk_lang_IO_print_I) },
    { "ydk/lang/IO", "print", "(C)V", FORCE(ydk_lang_IO_print_C) },
    { "java/lang/String", "append", "(I)V", FORCE(java_lang_string_append_I) },
    { "java/lang/String", "append", "(C)V", FORCE(java_lang_string_append_C) },
    { "java/lang/String", "append", "(Ljava/lang/String;)V", FORCE(java_lang_string_append_str) }
};

YVM::YVM() {
#ifdef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE
    Inspector::printSizeofInternalTypes();
#endif
}

bool YVM::loadClass(const char * name){
    return yrt.ma->loadJavaClass(name);
}

bool YVM::linkClass(const char * name){
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage exception to denote it;
        throw std::runtime_error("LinkageException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->linkJavaClass(name);
    return true;
}

bool YVM::initClass(const char * name){
    if (!yrt.ma->findJavaClass(name)) {
        // It's not an logical endurable error, so we throw and linkage exception to denote it;
        throw std::runtime_error("InitializationException: Class haven't been loaded into YVM yet!");
    }
    yrt.ma->initClassIfAbsent(exec,name);
    return true;
}

void YVM::callMain(const char * name) {
    auto * jc = yrt.ma->loadClassIfAbsent(name);
    yrt.ma->linkClassIfAbsent(name);
    yrt.ma->initClassIfAbsent(exec, name);
    exec.invokeByName(jc, "main", "([Ljava/lang/String;)V");
}

void YVM::registerNativeMethod(const char *className, const char * name, const char * descriptor,JType *(*func)(RuntimeEnv *env)) {
    std::string methodName(className);
    methodName.append(".");
    methodName.append(name);
    methodName.append(".");
    methodName.append(descriptor);
    yrt.nativeMethods.insert(std::make_pair(methodName, func));
}

void YVM::warmUp(const char  *libraryPath[], int howManySearchPath) {
    int p = sizeof nativeFunctionTable/sizeof nativeFunctionTable[0];
    for(int i=0;i<p;i++) {
        this->registerNativeMethod(
            nativeFunctionTable[i][0],
            nativeFunctionTable[i][1],
            nativeFunctionTable[i][2],
            reinterpret_cast<JType *(*)(RuntimeEnv *)>(
                const_cast<char*>(nativeFunctionTable[i][3])));
    }

    // initialize runtime environment's search paths
    yrt.ma = new MethodArea(libraryPath, howManySearchPath);
} 
