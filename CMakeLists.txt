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
    set(SOURCE_FILES src/Main.cpp src/MethodArea.cpp src/Frame.h src/ClassFile.h src/AccessFlag.h src/RuntimeEnv.cpp src/NativeMethod.h
            src/CodeExecution.cpp src/Debug.cpp src/JavaClass.cpp src/JavaHeap.cpp src/JavaHeap.hpp src/CodeExecution.hpp
            src/NativeMethod.cpp src/YVM.cpp src/Utils.h src/Utils.cpp src/JavaException.h src/JavaException.cpp src/ObjectMonitor.h 
            src/ObjectMonitor.cpp src/GC.h src/GC.cpp src/Option.h src/Concurrent.hpp src/Concurrent.cpp src/Internal.h)
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