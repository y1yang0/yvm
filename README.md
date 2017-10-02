# ![](https://github.com/racaljk/yvm/blob/master/public/y.png)![](https://github.com/racaljk/yvm/blob/master/public/v.png)![](https://github.com/racaljk/yvm/blob/master/public/m.png) YVM
用Java HotSpot VM(Server)开发的Java VM,一个小小的自举,目标是完全支持JDK1.8.0,
目前对效率没什么追求,所以代码执行引擎采用解释执行.


# Tower of Babel
- [x] 读取字节码并解析为运行时结构
- [x] 有限的字节码安全校验
- [ ] 完成代码执行引擎
- [ ] 完善安全检查，目前只实现了`*.class`文件格式的静态检查
- [ ] 必须支持本地方法调用
- [ ] 增加GC
- [ ] 代码执行引擎严格按照`JVM 8 SPEC`抛出异常
- [ ] 最终计划，用CPP重写，从玩具转型为实用VM
- [x] MetaClass去掉getter,setter
- [ ] Javadoc注释
- [ ] 支持synchronized临界
- [ ] 支持wide指令
- [ ] 单元测试
- [ ] 支持签名多态性方法

# Implementation Details
要想一个人完成Java虚拟机不是一件容易的事情，有很多交叉引用的数据需要合理的安排，需要自己设计运行时各种数据的表示
同时也要合理权衡，如果一上来就想完美实现SPEC提到的所有内容那造这虚拟机和修巴别塔就真的没区别了。
这里列出了一些涉及实现的权衡结果，但这不是最终方案，
我会陆续增补它们，所以，敬请期待吧:)

+ 数组惰性求值，直到写操作才分配内存。
+ 不打算支持`jsr`,`ret`和`jsr_w`指令，所以`JavaSE6`及其之前的编译器编译含有`finally clause`的代码无法在该虚拟机工作
+ 异常重抛用局部类递归实现，所以如果异常链非常非常长又没有任何捕获clause会导致栈溢出
+ `invokedynamic`是用于支持动态类型语言的Opcode，但是离目前能运行的Goal还有一段时间，所以
暂时不支持，但是我觉得挺有意思的，以后一定会添加上(runtime data已经获取了，只是opcode没有实现)
+ 浮点值在NaN/infinity等极值下的计算会造成VM crash，日后修复
+ 对于高级用户，需要注意在`*.class`中自定义的Attributes不会保留到运行时

# License
采用MIT协议发布
