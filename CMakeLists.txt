cmake_minimum_required(VERSION 3.5)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-O1)
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")

set(SOURCE_FILES src/main.cpp src/MethodArea.cpp src/Frame.h src/ClassFile.h src/AccessFlag.h src/CommandParser.h src/RuntimeEnv.cpp
        src/CodeExecution.cpp src/Debug.cpp src/Descriptor.cpp src/JavaClass.cpp src/JavaHeap.cpp src/CodeExecution.hpp src/NativeMethod.cpp
        src/YVM.cpp src/Utils.h src/Utils.cpp src/JavaException.h src/JavaException.cpp src/ObjectMonitor.h src/ObjectMonitor.cpp
        src/GC.h src/GC.cpp src/Option.h src/Concurrent.hpp src/Concurrent.cpp )
add_executable(yvm ${SOURCE_FILES})
target_link_libraries(yvm pthread)
