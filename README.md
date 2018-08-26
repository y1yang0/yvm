![](./public/dragon.png) Yet another java Virtual Machine

[中文](https://github.com/racaljk/yvm/blob/master/README.md) | [English](https://github.com/racaljk/yvm/blob/master/README.EN.md)
| [![Build Status](https://travis-ci.org/racaljk/yvm.svg?branch=master)](https://travis-ci.org/racaljk/yvm) | ![](https://img.shields.io/badge/comiler-MSVC2017-brightgreen.svg) | ![](https://img.shields.io/badge/comiler-gcc7.0-brightgreen.svg)

YVM是用C++写的一个Java虚拟机，现在支持Java大部分功能，以及一个基于"标记清除算法"的并发垃圾回收器. 不过还有很多bug等待修复。
感兴趣的朋友pull request/fork/star吧！

# 已支持语言特性
高级特性逐步支持中，可以开Issue提议或者直接PR
+ Java基本算术运算，流程控制语句，面向对象。
+ [RTTI](./javaclass/ydk/test/InstanceofTest.java)
+ [字符串拼接(+,+=符号重载)](./javaclass/ydk/test/StringConcatenation.java)
+ [异常处理(可输出stacktrace)](./javaclass/ydk/test/ThrowExceptionTest.java)
+ [创建异步线程](./javaclass/ydk/test/CreateAsyncThreadsTest.java)
+ [Synchronized(支持对象锁)](./javaclass/ydk/test/SynchronizedBlockTest.java)
+ [垃圾回收(标记清除算法)](./javaclass/ydk/test/GCTest.java)

# 构建和运行
编译依赖于Boost库,请在`CMakeLists.txt`中手动配置Boost库位置:
```
set(BOOST_ROOT "/your_boost_root_dir")
```
然后构建运行即可

1. `编译` 
```bash
$ cd yvm
$ cmake .
$ make -j4
$ make test
```
2. `运行`
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

# 关于JDK
部分JDK类是JVM运行攸关的,但由于JDK比较复杂不便于初期开发,所以这里用重写过的JDK代替,源码参见[javaclass](./javaclass)目录,可以使用`compilejava.bat`进行编译，编译后`*.class`文件位于[bytecode](./bytecode).
目前重写过的JDK类有:
+ `java.lang.String`
+ `java.lang.StringBuilder`
+ `java.lang.Throwable`
+ `java.lang.Math(::random())`
+ `java.lang.Runnable`
+ `java.lang.Thread`

# 运行效果
+ helloworld
![](./public/hw.png)
![](./public/helloworld.png)
+ 快速排序
![](./public/quicksort_java.png)
![](./public/quicksort_console.png)
+ 异常调用栈轨迹
![](./public/stj.png)
![](./public/stc.png)
+ 原生多线程
![](./public/without_synchronized_java.png)
![](./public/without_synchronized_console.png)
+ Synchronized保护下的多线程
![](./public/synchronized_java.png)
![](./public/synchronized_console.png)
+ 垃圾回收
![](./public/gc_java.png)
![](./public/gc_sampling_2.png)

# 开发文档
[Wiki](https://github.com/racaljk/yvm/wiki)中有很多详细的开发文档，如果想探索关于`YVM`的更多内容，请移步浏览:-)


# License
Code licensed under the MIT License.
