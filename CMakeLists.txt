cmake_minimum_required(VERSION 3.5)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
endif()

set(SOURCE_FILES src/vm/Main.cpp src/runtime/MethodArea.cpp src/runtime/JavaFrame.hpp src/runtime/JavaFrame.cpp src/classfile/ClassFile.h src/classfile/AccessFlag.h src/runtime/RuntimeEnv.cpp src/misc/NativeMethod.h
        src/interpreter/Interpreter.cpp src/interpreter/SymbolicRef.cpp src/misc/Debug.cpp src/runtime/JavaClass.cpp src/runtime/JavaHeap.cpp src/runtime/JavaHeap.hpp src/interpreter/Interpreter.hpp src/interpreter/MethodResolve.cpp
        src/misc/NativeMethod.cpp src/vm/YVM.cpp src/misc/Utils.h src/misc/Utils.cpp src/runtime/JavaException.h src/runtime/JavaException.cpp src/runtime/ObjectMonitor.h
        src/runtime/ObjectMonitor.cpp src/gc/GC.h src/gc/GC.cpp src/misc/Option.h src/gc/Concurrent.hpp src/gc/Concurrent.cpp src/interpreter/Internal.h src/interpreter/CallSite.cpp)
add_executable(yvm ${SOURCE_FILES})
if(UNIX)
    target_link_libraries(yvm pthread)
endif()