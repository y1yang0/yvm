# YVM
这是用C++写的一个虚拟机，现在支持java大部分指令，不过还有很多bug等待修复。
感兴趣的朋友pull request/fork/star吧！

# 使用
1. 配置src/main.cpp
```cpp
#include "YVM.h"

int main() {
    const char * ydk[] = {
    	//该项目java lib文件所在的绝对路径
        R"(C:\Users\Cthulhu\Desktop\yvm\javalib)"
    };

    YVM vm;
    vm.warmUp(ydk, sizeof(ydk) / sizeof(ydk[0]));
    vm.callMain("ydk/test/InheritanceTest");
    system("pause");
    return 0;
}
```
2. 支持G++7.0,MSVC 2017,直接编译运行即可

# 效果
0. helloworld
![](./public/hw.png)
![](./public/helloworld.png)
1. 冒泡排序
![](./public/bubbletest.png)
![](./public/bbb.png)


# 限制和TODO
0. YVM现在不支持JavaSE6及其之前版本编译器编译的`.class`
1. **不能使用任何JDK的方法**,不过部分运行攸关的JDK类(如String,StringBuilder)已经重写过。

# 公有设计，私有实现
1. YVM中类只有一种权威表示,即"package/foo/Bar",任何其他表示如"java.lang.Object"都拒绝加载
2. 根据JVM SPEC,虚拟机执行引擎会直接操纵JDK一些类,如`java/lang/Class`,,`java/lang/String`.为了实现一个"Runnable"的JVM，这里使用重写的同名类(位于javalib)代替。


# License
Code licensed under the MIT License.