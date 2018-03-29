# YVM
去年我用Java写过一个半成品的jvm(参见commit history)，后期由于数据结构设计错误导致代码执行引擎一次，如果修改结构几乎是推导重来，于是无奈放弃。
这学期时间很充裕，在推倒重来用C++写前我已经设计过大概的数据结构，多了几分思考，少了几分编码速度。
感兴趣的朋友fork/star/pr吧！

#

# 限制
0. YVM现在不支持JavaSE6及其之前版本编译器编译的`.class`
1. 不支持`synchronized`
2. 不能使用任何JDK的方法,请使用`ydk.lang.*`(如果使用一个jdk方法，基本就得实现大部分jdk库,但仅靠一个人的力量写JDK难如登天，这和我的初衷(造一个可用的娱乐JVM)有悖)

# 公有设计，私有实现
0. 在JVM中`byte`,`char`,`boolean`,`short`都会进行符号扩展至`int`,即JVM实际存在的类型只有`int`,`long`,`float,`double`,`object ref`,`array`
1. YVM中类只有一种权威表示,即"package/foo/Bar",任何其他表示如"java.lang.Object"都拒绝加载
2. 根据JVMSPEC,虚拟机执行引擎会直接操纵JDK一些类,如`java/lang/Class`,`java/lang/invoke/MethodType`,`java/lang/invoke/MethodHandle`,`java/lang/String`.为了实现一个"Runnable"的JVM，这里使用自建YDK的类代替它们。


# License
Code licensed under the MIT License.