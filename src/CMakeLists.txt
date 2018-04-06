cmake_minimum_required(VERSION 3.8)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-O3)

set(SOURCE_FILES main.cpp MethodArea.cpp Frame.h ClassFile.h AccessFlag.h CommandParser.h ConstantTag.h RuntimeEnv.cpp
        CodeExecution.cpp Debug.cpp Descriptor.cpp JavaClass.cpp JavaHeap.cpp CodeExecution.hpp NativeMethod.cpp
        YVM.cpp Utils.h Utils.cpp JavaException.h JavaException.cpp ObjectMonitor.h ObjectMonitor.cpp)
add_executable(yvm ${SOURCE_FILES})