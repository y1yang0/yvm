# YVM
## Introduction

> [中文](./README.ZH.md) | [English](./README.md)

This is a toy Java virtual machine(JVM) that supports many of Java's language features, it also comes with an experimental `mark-sweep` garbage collector. The implementation of this JVM follows the [Java Virtual Machine Specification 8](https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf). I don't have enough time to write unit tests for each component, and I don't implement the full Java language features, but as far as pleasing myself, I think it's enough. If you find any bugs, or you want to implement any missing features, you can create an issue or pull request, respectively.

## Build and Run
```bash
# Note, C++14 is required at least.
$ cd yvm
$ cmake .
$ make
$ ./yvm
Usage:
  yvm --lib=<path> <main_class>

      --lib=<path>     Tells YVM where to find JDK classes(java.lang.String, etc)
      <main_class>     The full qualified Java class name, e.g. org.example.Foo
$ ./yvm --lib=/path/to/yvm/bytecode ydk.test.QuickSort
0 1 1 1 1 1 4 4 4 5 6 7 7 9 9 9 12 74 96 98 8989 
```

## Implemented Features
Advanced language features will support later, you can also create PR to contribute your awesome code.
Implemented features are as follows:
+ Java arithmetic, flow control, object-oriented programming(virtual method, inherit,etc.)
+ [Runtime type identification](./javaclass/ydk/test/InstanceofTest.java)
+ [String concatenation](./javaclass/ydk/test/StringConcatenation.java)
+ [Exception handling](./javaclass/ydk/test/ThrowExceptionTest.java)
+ [Async native threads](./javaclass/ydk/test/CreateAsyncThreadsTest.java)
+ [Synchronized block with object lock](./javaclass/ydk/test/SynchronizedBlockTest.java)
+ [Garbage Collection(With mark-and-sweep policy)](./javaclass/ydk/test/GCTest.java)

![](./docs/snapshot.jpg)

You can find some examples at [here](javaclass/ydk/test/).

## Hacking Guide
### 1. How does it work
1. `loadJavaClass("org.example.Foo")`
    - findJavaClass if present
    - Otherwise, loadJavaClass from --lib, all classes is stored in ClassSpace
2. `linkJavaClass("org.example.Foo")`
    - Initialize static fields with default value
3. `initJavaClass("org.example.Foo")`
    - Invoke `org.example.Foo.<clinit>`
4. `invokeByName("org.example.Foo","main","([Ljava/lang/String;)V")`
    - Prepare execution frame
    - Find method from JavaClass
    - `execByteCodede`
        - Interprete every bytecode on simulated stack
        - Recursively call `execByteCodede` when performing `invoke*` bytecodes

### 2. Code structure
```bash
root@ubuntu:~/yvm/src$ tree .
.
├── classfile               
│   ├── AccessFlag.h        # Access flag of class, method, field
│   ├── ClassFile.h         # Parse .class file
│   └── FileReader.h        # Read .class file
├── gc
│   ├── Concurrent.cpp      # Concurrency utilities
│   ├── Concurrent.hpp
│   ├── GC.cpp              # Garbage collector
│   └── GC.h
├── interpreter
│   ├── CallSite.cpp        # Call site to denote a concrete calling
│   ├── CallSite.h
│   ├── Internal.h          # Types that internally used
│   ├── Interpreter.cpp     # Interpreter
│   ├── Interpreter.hpp
│   ├── MethodResolve.cpp   # Resolve call site
│   └── MethodResolve.h
├── misc
│   ├── Debug.cpp           # Debugging utilities
│   ├── Debug.h
│   ├── NativeMethod.cpp    # Java native methods
│   ├── NativeMethod.h
│   ├── Option.h            # VM arguments and options
│   ├── Utils.cpp           # Tools and utilities
│   └── Utils.h
├── runtime
│   ├── JavaClass.cpp       # Internal representation of java.lang.Class
│   ├── JavaClass.h
│   ├── JavaException.cpp   # Exception handling
│   ├── JavaException.h
│   ├── JavaFrame.cpp       # Execution frame
│   ├── JavaFrame.hpp
│   ├── JavaHeap.cpp        # Java heap, where objects are located
│   ├── JavaHeap.hpp
│   ├── JavaType.h          # Java type definitions
│   ├── ClassSpace.cpp      # Store JavaClass
│   ├── ClassSpace.h
│   ├── ObjectMonitor.cpp   # synchronized(){} block implementation
│   ├── ObjectMonitor.h
│   ├── RuntimeEnv.cpp      # Runtime structures
│   └── RuntimeEnv.h
└── vm
    ├── Main.cpp             # Parse command line arguments
    ├── YVM.cpp              # Abstraction of virtual machine
    └── YVM.h
```
See its wiki for more development details.

## License
Code licensed under the MIT License.
