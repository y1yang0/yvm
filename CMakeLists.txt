cmake_minimum_required(VERSION 3.5)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-O0)
if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
endif()

# YOU MAY CHANGE THESE OPTIONS TO ADAPT YOUR ENVIRONMENT
# BOOST LIBRARY
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(BOOST_ROOT "F:\\cpplib\\boost_1_65_1")
find_package(Boost COMPONENTS system filesystem program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(SOURCE_FILES src/Main.cpp src/MethodArea.cpp src/Frame.h src/ClassFile.h src/AccessFlag.h src/CommandParser.h src/RuntimeEnv.cpp
            src/CodeExecution.cpp src/Debug.cpp src/Descriptor.cpp src/JavaClass.cpp src/JavaHeap.cpp src/CodeExecution.hpp src/NativeMethod.cpp
            src/YVM.cpp src/Utils.h src/Utils.cpp src/JavaException.h src/JavaException.cpp src/ObjectMonitor.h src/ObjectMonitor.cpp
            src/GC.h src/GC.cpp src/Option.h src/Concurrent.hpp src/Concurrent.cpp )
    add_executable(yvm ${SOURCE_FILES})
    link_directories(... ${Boost_LIBRARY_DIRS})
    target_link_libraries(yvm ${Boost_LIBRARIES})
    if(UNIX)
        target_link_libraries(yvm lpthread)
    endif()
endif()


enable_testing()
if(WIN32)
    add_test(NAME test_running_help COMMAND yvm --help)
    add_test(NAME test_running_BaseTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.BaseTest)
    #add_test(NAME test_running_CreateAsyncThreadsTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.CreateAsyncThreadsTest)
    #add_test(NAME test_running_GCTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.GCTest)
    add_test(NAME test_running_GenericException COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.GenericException)
    add_test(NAME test_running_InstanceofTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.InstanceofTest)
    add_test(NAME test_running_MathTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.MathTest)
    add_test(NAME test_running_ObjectArrayTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.ObjectArrayTest)
    add_test(NAME test_running_QuickSort COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.QuickSort)
    add_test(NAME test_running_StackTraceTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.StackTraceTest)
    add_test(NAME test_running_StringConcatenation COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.StringConcatenation)
    #add_test(NAME test_running_SynchronizedBlockTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.SynchronizedBlockTest)
    add_test(NAME test_running_ThrowExceptionTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.ThrowExceptionTest)
    add_test(NAME test_running_WithoutReasonException COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.WithoutReasonException)
    #add_test(NAME test_running_WithoutSynchronizedBlockTest COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.WithoutSynchronizedBlockTest)
    add_test(NAME test_running_WithReasonException COMMAND yvm --runtime=${PROJECT_SOURCE_DIR}/bytecode ydk.test.WithReasonException)
endif()

