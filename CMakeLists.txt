cmake_minimum_required(VERSION 3.5)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
endif()

# YOU *SHOULD* CHANGE THESE OPTIONS TO ADAPT YOUR ENVIRONMENT
# BOOST LIBRARY
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(BOOST_ROOT "F:\\cpplib\\boost_1_65_1")
find_package(Boost COMPONENTS system filesystem program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(SOURCE_FILES src/vm/Main.cpp src/runtime/MethodArea.cpp src/runtime/JavaFrame.hpp src/runtime/JavaFrame.cpp src/classfile/ClassFile.h src/classfile/AccessFlag.h src/runtime/RuntimeEnv.cpp src/misc/NativeMethod.h
            src/interpreter/Interpreter.cpp src/misc/Debug.cpp src/runtime/JavaClass.cpp src/runtime/JavaHeap.cpp src/runtime/JavaHeap.hpp src/interpreter/Interpreter.hpp src/interpreter/MethodResolve.cpp
            src/misc/NativeMethod.cpp src/vm/YVM.cpp src/misc/Utils.h src/misc/Utils.cpp src/runtime/JavaException.h src/runtime/JavaException.cpp src/runtime/ObjectMonitor.h
            src/runtime/ObjectMonitor.cpp src/gc/GC.h src/gc/GC.cpp src/misc/Option.h src/gc/Concurrent.hpp src/gc/Concurrent.cpp src/interpreter/Internal.h src/interpreter/CallSite.cpp)
    add_executable(yvm ${SOURCE_FILES})
    link_directories(... ${Boost_LIBRARY_DIRS})
    target_link_libraries(yvm ${Boost_LIBRARIES})
    if(UNIX)
        target_link_libraries(yvm lpthread)
    endif()
endif()


enable_testing()
file(GLOB test_file_name ${PROJECT_SOURCE_DIR}/javaclass/ydk/test/*.java)

if(WIN32)
    # adhoc tests
    add_test(NAME test_help COMMAND yvm --help)

    # automatically detected tests
    foreach(each_file ${test_file_name})
        string(REGEX REPLACE ".*/(.*)\.java" "\\1" curated_name ${each_file})
        add_test(NAME test_${curated_name} COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode "ydk.test.${curated_name}")
    endforeach(each_file ${test_file_name})
endif()