cmake_minimum_required(VERSION 3.5)
project(yvm)

set(CMAKE_CXX_STANDARD 14)
if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
endif()

# Compile and link together
file(GLOB_RECURSE YVM_SRC src/**.cpp)
add_executable(yvm ${YVM_SRC})

enable_testing()
file(GLOB test_file_namea ${PROJECT_SOURCE_DIR}/javaclass/ydk/test/*.java)

# Create unit tests
foreach(each_file ${test_file_namea})
    string(REGEX REPLACE ".*/(.*)\\.java" "\\1" curated_name ${each_file})
    add_test(NAME example_${curated_name} COMMAND yvm --lib=${PROJECT_SOURCE_DIR}/bytecode "ydk.test.${curated_name}")
endforeach(each_file ${test_file_namea})
