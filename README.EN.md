![](./public/dragon.png) Yet another java Virtual Machine

[中文](https://github.com/racaljk/yvm/blob/master/README.md) | [English](https://github.com/racaljk/yvm/blob/master/README.EN.md)
| [![Build Status](https://travis-ci.org/racaljk/yvm.svg?branch=master)](https://travis-ci.org/racaljk/yvm) | ![](https://img.shields.io/badge/comiler-MSVC2017-brightgreen.svg) | ![](https://img.shields.io/badge/comiler-gcc7.0-brightgreen.svg)


This is a homemade Java virtual machine written in c++, it supports most Java language features and includes a mark-sweep-based concurrent garbage collector. The main components of this VM are conform to [Java Virtual Machine Specification 8](https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf). Now it is runnable and sorts of language features will add into this VM in progress. I don't have enough time to write unittests to verify all aspects of yvm, so if you find any bugs, you can open an [Issue](https://github.com/racaljk/yvm/issues/new) or fix up in place and pull request directly.

# Available language features
Advanced language features will support later, you can also PR to contribute your awesome code.
+ Java arithmetic, flow control, object-oriented programming(virtual method, inherit,etc.)
+ [Runtime type identification](./javaclass/ydk/test/InstanceofTest.java)
+ [String concatenation](./javaclass/ydk/test/StringConcatenation.java)
+ [Exception handling](./javaclass/ydk/test/ThrowExceptionTest.java)
+ [Async native threads](./javaclass/ydk/test/CreateAsyncThreadsTest.java)
+ [Synchronized block with object lock](./javaclass/ydk/test/SynchronizedBlockTest.java)
+ [Garbage Collection(With mark-and-sweep policy)](./javaclass/ydk/test/GCTest.java)

# Build and run
**This project depends on Boost libraries.** You need to manually configure Boost root path in `CMakeLists.txt`:
```
set(BOOST_ROOT "/your_boost_root_dir")
```
Thats all about to say. The following steps are so common:

1. `Compile`
```bash
$ cd yvm
$ cmake .
$ make -j4
$ make test
```
2. `Run`
```bash
$ ./yvm -h
Yvm - Yet another java Virtual Machine :)
Usage:
./yvm [option|option=value] program_name

option:
-h or --help                    List help documentations and usages.
-rt or --runtime(required)                      Attach java runtime libraries of this YVM.
-sp or --searchpath                      Add *.class to search path.

You must specify the "runtime" flag to tell yvm where it could find jdk classes, and also program name is required.

$ ./yvm --runtime=C:\Users\Cthulhu\Desktop\yvm\bytecode ydk.test.QuickSort
```

# About JDK
Any java virtual machines can not run a Java program without Java libraries. As you may know, some opcodes like `ldc`,`monitorenter/monitorexit`,`athrow` are internally requiring our virtual machine to operate JDK classes(`java.lang.Class`,`java.lang.String`,`java.lang.Throwable`,etc). Hence, I have to rewrite some [JDK classes](./javaclass) for building a runnable VM , because original JDK classes are so complicated that it's inconvenient for early developing.
Rewrote JDK classes are as follows:
+ `java.lang.String`
+ `java.lang.StringBuilder`
+ `java.lang.Throwable`
+ `java.lang.Math(::random())`
+ `java.lang.Runnable`
+ `java.lang.Thread`

# Running snapshots
+ helloworld
![](./public/hw.png)
![](./public/helloworld.png)
+ quick sort
![](./public/quicksort_java.png)
![](./public/quicksort_console.png)
+ print stack trace when exception occurred
![](./public/stj.png)
![](./public/stc.png)
+ native multithreading
![](./public/without_synchronized_java.png)
![](./public/without_synchronized_console.png)
+ multithreading with synchronized(){}
![](./public/synchronized_java.png)
![](./public/synchronized_console.png)
+ Garbage Collection
![](./public/gc_java.png)
![](./public/gc_sampling_2.png)

# Development docs
For more VM development documentation, see its [Wiki](https://github.com/racaljk/yvm/wiki), which contains various contents with regard to VM structures, usages, and design principal, etc.  

# License
Code licensed under the MIT License.
