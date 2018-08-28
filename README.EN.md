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

# 构建和运行
+ Prerequisite
  + [Boost](https://www.boost.org/)(>=1.65) 请在`CMakeLists.txt`中手动配置Boost库位置
  + CMake(>=3.5)
  + C++14
  + gcc/msvc/mingw均可
+ Stereotype
```bash
$ cd yvm
$ cmake .
$ make -j4
$ make test
```
```bash
$ ./yvm --help
Usage:
  --help                List help documentations and usages.
  --runtime arg         Attach java runtime libraries where yvm would lookup 
                        classes at
  --run arg             Program which would be executed soon
You must specify the "runtime" flag to tell yvm where it could find jdk classes, and also program name is required.
$ ./yvm --runtime=C:\Users\Cthulhu\Desktop\yvm\bytecode ydk.test.QuickSort
```

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

# About JDK
Any java virtual machines can not run a Java program without Java libraries. As you may know, some opcodes like `ldc`,`monitorenter/monitorexit`,`athrow` are internally requiring our virtual machine to operate JDK classes(`java.lang.Class`,`java.lang.String`,`java.lang.Throwable`,etc). Hence, I have to rewrite some [JDK classes](./javaclass) for building a runnable VM , because original JDK classes are so complicated that it's inconvenient for early developing.
Rewrote JDK classes are as follows:
+ `java.lang.String`
+ `java.lang.StringBuilder`
+ `java.lang.Throwable`
+ `java.lang.Math(::random())`
+ `java.lang.Runnable`
+ `java.lang.Thread`

# Development docs
## Internal object and array representations
```cpp
// Java heap holds instance's fields data which object referred to and elements
// of an array. This is the core component of yvm, almost every memory
// storage/access/deletion took place here.
//
// The ObjectContainer manages object's fields data, the key also the
// only way to identify an object is the offset, you can use offset to get
// object's fields data, push a field data into objects which specified by
// offset and place a new area to store object fields Here is the internal
// construction:
//
// [1]  ->  [field_a, field_b, field_c]
// [2]  ->  []
// [3]  ->  [field_a,field_b]
// [4]  ->  [field_a]
// [..] ->  [...]

// The ArrayContainer manages array's elements as well as object pool
//
// [1]  ->   <3, [field_a, field_b, field_c]>
// [2]  ->   <0, []>
// [3]  ->   <2, [field_a,field_b]>
// [4]  ->   <1, [field_a]>
// [..] ->   <..,[...]>

// MonitorContainer manages synchronous block monitors as well as the
// above
//
// [1]  ->   ObjectMonitor*
// [2]  ->   ObjectMonitor*
// [3]  ->   ObjectMonitor*
// [4]  ->   ObjectMonitor*
// [..] ->   ObjectMonitor*

```
For more VM development documentation, see its [Wiki](https://github.com/racaljk/yvm/wiki) or source code comments, which contains various contents with regard to VM structures, usages, and design principal, etc.  

# License
Code licensed under the MIT License.
