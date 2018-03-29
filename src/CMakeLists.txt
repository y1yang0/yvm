cmake_minimum_required(VERSION 3.8)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-O3)

set(SOURCE_FILES main.cpp MethodArea.cpp RuntimeEnv.cpp CodeExecution.cpp Debug.cpp Descriptor.cpp JavaClass.cpp JavaHeap.cpp CodeExecution.hpp NativeMethod.cpp YVM.cpp)
add_executable(yvm ${SOURCE_FILES})