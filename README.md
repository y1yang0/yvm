# YVM
用Java HotSpot VM(Server)开发的Java VM,一个小小的自举,目标是完全支持JDK1.8.0,
目前对效率没什么追求,所以代码执行引擎采用解释执行.


# Tower of Babel
- [x] 读取字节码并解析为运行时结构
- [x] 有限的字节码安全校验
- [ ] 完成代码执行引擎
- [ ] 完善安全检查，目前只实现了`*.class`文件格式的静态检查
- [ ] 支持本地方法调用
- [ ] 增加GC
- [ ] 代码执行引擎严格按照`JVM 8 SPEC`抛出异常
- [ ] 最终计划，用CPP重写，从玩具转型为实用VM

# Bugs to be fixed
- [ ] 浮点值在NaN/infinity等极值下的计算会造成VM crash

# Improvement
- [x] MetaClass去掉getter,setter
- [ ] Javadoc注释

# Features or Faults
+ 数组惰性求值，直到写操作才分配内存。
+ 不打算支持`jsr`,`ret`和`jsr_w`指令，所以`JavaSE6`及其之前的编译器编译含有`finally clause`的代码无法在该虚拟机工作
+ 异常重抛用局部类递归实现，所以如果异常链非常非常长又没有任何捕获clause会导致栈溢出

# License
目前采用MIT协议发布
