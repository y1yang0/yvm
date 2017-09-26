# YVM
用Java HotSpot VM(Server)开发的Java VM,一个小小的自举,支持JDK1.8.0,
对效率没什么追求,所以代码执行引擎采用解释执行.(以后大概率Cpp重写，线程库在造中)

# Plan
- [ ] 完成代码执行引擎
- [ ] 完善安全检查，目前只实现了`*.class`文件格式的静态检查
- [ ] 支持本地方法调用
- [ ] 增加GC
- [ ] 代码执行引擎严格按照`JVM 8 SPEC`抛出异常

# Bugs to be fixed
- [ ] 浮点值在NaN/infinity等极值下的计算会造成VM crash

# License
目前采用MIT协议发布
