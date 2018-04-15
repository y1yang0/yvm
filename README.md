![](./public/banner.jpg)
[中文](https://github.com/racaljk/yvm/blob/master/README.md) | [English](https://github.com/racaljk/yvm/blob/master/README.EN.md)

YVM是用C++写的一个Java虚拟机，现在支持Java大部分功能，以及一个基于"标记清除算法"的并发垃圾回收器. 不过还有很多bug等待修复。
感兴趣的朋友pull request/fork/star吧！

# 已支持语言特性
高级特性逐步支持中，可以开Issue提议或者直接PR
+ Java基本算术运算，流程控制语句，面向对象。
+ [RTTI](./javalib_src/ydk/test/InstanceofTest.java)
+ [字符串拼接(+,+=符号重载)](./javalib_src/ydk/test/StringConcatenation.java)
+ [异常处理(可输出stacktrace)](./javalib_src/ydk/test/ThrowExceptionTest.java)
+ [创建异步线程](./javalib_src/ydk/test/CreateAsyncThreadsTest.java)
+ [Synchronized(支持对象锁)](./javalib_src/ydk/test/SynchronizedBlockTest.java)
+ [垃圾回收(标记清除算法)](./javalib_src/ydk/test/GCTest.java)

# 构建和运行
预置MSVC 2017工程文件,同时支持CMakeLists
1. `配置`
编辑`src/Option.h`,如果是Windows则
```cpp
#define TARGET_WIN32
```
如果是Linux则
```cpp
#define TARGET_LINUX
```
2. `编译` 如果使用MSVC,直接打开`src/yvm.sln`即可
如果使用CMakeLists
```bash
$ cd yvm
$ cmake .
$ make -j4
```
2. `运行`
```bash
# --runtime为在YVM上运行的Java程序所必须的运行时,多个值用";"分隔
# 后面指定运行程序的全修饰名，如ydk.test.QuickSort
./yvm --runtime=C:\Users\Cthulhu\Desktop\yvm\javalib ydk.test.QuickSort
```

# 关于JDK
部分JDK类是JVM运行攸关的,但由于JDK比较复杂不便于初期开发,所以这里用重写过的JDK代替,源码参见[javalib_src](./javalib_src)目录,编译后`*.class`文件位于[javalib](./javalib)
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