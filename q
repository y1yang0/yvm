[33mcommit 9e2ea16e86fc60e84c92bb2bbaf7320e9da84ba9[m[33m ([m[1;36mHEAD -> [m[1;32mmaster[m[33m, [m[1;31morigin/master[m[33m, [m[1;31morigin/HEAD[m[33m)[m
Author: racaljk <1948638989@qq.com>
Date:   Wed Aug 29 10:49:47 2018 +0800

    bug fix: native print would crash if passed argument is null

[33mcommit b8508f2c1074d21cd854cc7350978c4adf15ce55[m
Author: racaljk <1948638989@qq.com>
Date:   Tue Aug 28 21:52:34 2018 +0800

    bug fix:if derived class field which has the same name with base class, the later would be inaccessible

[33mcommit 50c2e68548d99bf64ccaa7b2388979ca0578c820[m
Author: racaljk <1948638989@qq.com>
Date:   Tue Aug 28 21:14:46 2018 +0800

    heap: fix field access bug
    
    heap: rename APIs
    global: code format
    readme: add internal implementation documentation

[33mcommit 4be17470831e2a7ced15b7b12ee18a3845ca3b13[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Aug 27 19:40:25 2018 +0800

    global: instead of mixing cstyle string and std string , now all functions accept and return std::string

[33mcommit d5a6ffa35e3dc22c110b648a8f8b4b528e15d1f2[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Aug 27 14:40:36 2018 +0800

    cmakelist: automatically detecting test files
    
    gctest: fit to a small threshold, becuase large number is not convenient for testing

[33mcommit a68a0655bcc303f1c2ffe2f8de3dbbd3b94889bb[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Aug 26 18:33:15 2018 +0800

    parser: replace old jerk implementation with boost.program_options

[33mcommit b46ebc35851ea36bc748a9bff87a7a3a8368f6e0[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Aug 26 17:05:05 2018 +0800

    method are: introduce boost.filesystem to eliminate os dependant code

[33mcommit 1b29b60e888d06d976e002b98c64912528d47bbf[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Aug 26 12:40:58 2018 +0800

    format: format code by clang-format
    
    travis ci: continuation integration of testing

[33mcommit b0e08b20ffc0da4c2ff25a0dcb5822311012862c[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Aug 26 12:25:53 2018 +0800

    test:  automated unit tests are available now
    
    structure: rename javalib_src -> javaclass, javalib -> bytecode
    project file: remove solution file of visual studio, I recommend to use cmake with vs or clion ide
    CodeExecution: change native method execution function signature
    readme: use new logo:)

[33mcommit cf350a8a4260294fcd64a56296c6ab1940740463[m
Author: racaljk <1948638989@qq.com>
Date:   Wed Apr 18 19:40:58 2018 +0800

    ConstantTag: remove redundant file

[33mcommit 178c05740eb369eb4fe7ad9aa92cb33c19a74da8[m
Author: racaljk <1948638989@qq.com>
Date:   Wed Apr 18 19:31:11 2018 +0800

    GC: fix safepoint reseting bug
    Concurrent: GC threadpool and execution thread pool derive from its base structure

[33mcommit 75d8610d46282eb9bbf254ab7a42b13f54e0a65b[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 18:21:48 2018 +0800

    GC: executing gc when all threads reached safepoint(Stop the world)
    
    Concurrent: rewrite thread pool according to a realistic demand
    
    YVM: spawn new thread before code execution
    
    NativeMethod: spawn new thread before code execution
    
    README: typo

[33mcommit f3a4b5e6446edc41808fde819ee889ed1c907360[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:39:35 2018 +0800

    README: add build&&compilers badage

[33mcommit cfae6917c6032b2211482fb2460651cd8cf8f180[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:32:15 2018 +0800

    Update .travis.yml

[33mcommit d7fbc4a3dc4831215c6cc0def88ba9f1bd4c9c2a[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:26:23 2018 +0800

    Update travis.yml

[33mcommit 1f093847fc69031d52aa4966c090d156268c2368[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:16:17 2018 +0800

    Update .travis.yml

[33mcommit daf3c0d7ed5bfb2bb8531806752af02df0295f38[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:07:47 2018 +0800

    Update .travis.yml

[33mcommit 42cf663d5f9306883283929a76d4c2e57e90ce33[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 11:02:41 2018 +0800

    Update .travis.yml

[33mcommit f6fa18934d2579ea360be02b440295b77e963a6e[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 10:55:15 2018 +0800

    Update .travis.yml

[33mcommit ec8642422a0a6220f488bcaddcde89b73f8a59d3[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 16 10:39:27 2018 +0800

    Update .travis.yml

[33mcommit 62177a358cb5701bd51b20b6e2f8b6b11dadf4fe[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Apr 15 22:25:55 2018 +0800

    Update .travis.yml

[33mcommit b9d8c25032aec7f5b3219b058af32e82f58becdd[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 19:32:18 2018 +0800

    TravisCI: add .travis.yml

[33mcommit b30e3d784cbfd1800a9455cd1b5272b12629d049[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 18:44:24 2018 +0800

    CMakeLists: update

[33mcommit 9beb1be32acf8a054d8932325e03791f2e63f815[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 18:15:41 2018 +0800

    Concurrent: typo

[33mcommit c51f83da30a219043e85dbe6575b184044873061[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 18:14:53 2018 +0800

    README : move development documentation to wiki pages
    Concurrent: typo

[33mcommit decf040fb7c5baf1aec63469aea4c13936fe4c51[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 17:35:26 2018 +0800

    README: update memory && cpu analysis graph

[33mcommit cec2cd74d0a38a33f9cf87f73bc9d3c30b0b1aa2[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 17:25:41 2018 +0800

    GC Stop the world: stop the world when gc started
    
    GC SafePoints: place gc on safe points(Currently they were all after method calling)
    
    Spinlock: move spinlock implementation from Frame.h to Concurrent.hpp
    
    GC: fix local variable table deallocation bug on marking phase(Fatal error)
    
    GC: rename to ConcurrentGC to represent its high performance :)
    
    Option: decrease gc threshold value, now the default value is 30M

[33mcommit 69c39eed145d5e31def76e7ddff2f817381d8334[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 15 13:25:28 2018 +0800

    YVM : use new synchronization mechanism(std::future) to replace old condition variable based operations
    
    RuntimeEnv: remove corresponding fields
    
    NativeMethod: modify corresponding thread spawning operation
    
    JavaClass&&CodeExecution : typo fixed

[33mcommit fdb30094fecb67437aa701b602b0b0d9d2c3efc1[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 21:53:02 2018 +0800

    CodeExecution: fix local variable table resizing bug

[33mcommit 66813b2bda817ad2a3ea4bd1b51a00e7ce4f0c8a[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 19:51:45 2018 +0800

    README: modify gc description

[33mcommit 98a24e9804bf35fb420c4e3b4ecb0abceb8955ce[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 19:49:29 2018 +0800

    GC: improve performance a lot
    
    Concurrent: create thread pool to group similar tasks
    
    Java Heap: narrow global heap mutex to somewhat fine-grained mutex;

[33mcommit cf871087a39541ac693d418b7690a12b4bff1c14[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 11:41:16 2018 +0800

    StackFrame : give multithreading-safe guarantee by spin lcok

[33mcommit 6ba78781928eb2e0c122a0a80bcb9bbacf5aa1b7[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 10:02:27 2018 +0800

    Public: resize gc_sampling picture

[33mcommit c253a5c955222abea3e8cc831dfcc3fe2ab64e3c[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 14 09:57:13 2018 +0800

    GC : fix array deallocation bug

[33mcommit 493cf48a5650404553f14344622e146f49bd9786[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 13 23:22:03 2018 +0800

    README: rewrite [build and run] section for CMakeLists

[33mcommit c949ae6e17e1a48c0e335dac345b3f65fa1e1636[m
Author: cthulhujk <yangyijk@126.com>
Date:   Fri Apr 13 07:49:02 2018 -0700

    Linux: we can build it on linux platform now

[33mcommit 35ae5ec6f1213045651543f771ca3b27ce9d1f42[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 13 19:59:58 2018 +0800

    README:  specify dotted-decorated Java program name to run

[33mcommit 79785ab61b25224fd218e13ede4628dce6262b10[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 13 19:50:28 2018 +0800

    MethodArea: use unorder containers instread of associative containers since underlying structure of unordered containers were usually a hashtable

[33mcommit f70595581a531b6aaa882591a7c3448a9de70496[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 13 19:40:06 2018 +0800

    GC : release object monitors when they were unused and start GC::gc() when the memory allocation was beyond a specific threshold value

[33mcommit 1076e0ebb3b4ee7eab71dce1731175fa2deeae46[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 12 22:30:06 2018 +0800

    README: a brief mark-and-sweep GC

[33mcommit 0525120519186be855ff4e7621d2b83f275fd26b[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 12 22:19:32 2018 +0800

    GC: here we use mark-and-sweep algorithm to release unused memory

[33mcommit 0157ed44cad2c4cf56e1c92a5220f1a04474026e[m
Author: racaljk <1948638989@qq.com>
Date:   Wed Apr 11 11:42:38 2018 +0800

    README: update a new banner:)

[33mcommit 17aae1e84eae60ba60f7885a999bdf1c90c8b978[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 7 18:53:47 2018 +0800

    JavaHeap: hide last parameter of  put/get obejct field methods

[33mcommit 49189ec6c965257eaa0efa9339f5de6b8b69f664[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 7 14:47:35 2018 +0800

    Utils: move IS_ATTR_* && IS_STACKFRAME_* to ClassFile.h

[33mcommit a5e0c436633f42e9aca44ffed395bb3a68d983ed[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Apr 7 11:18:47 2018 +0800

    ObjectArray: fix object array parsing error

[33mcommit cecfcf3a08e12962c34d681e1c05dde7a7c06990[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 6 22:18:17 2018 +0800

    YVM: move registerNativeMethod to Util.h

[33mcommit e93b88789a6b9a7fc3b64739134c1da13f507281[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 6 21:57:32 2018 +0800

    Concurrency: support synchronized(){} with object lock

[33mcommit 646045679f3938d6bf0850686699efa64963d635[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 6 19:50:28 2018 +0800

    Frame: fix a bad frame deallocating bug in its destructor

[33mcommit 4b499d5d327e784f79ce5a2f995e2e169ab285ff[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 6 14:40:42 2018 +0800

    CodeExecution：use callable object to eliminate redundant code

[33mcommit d4dbd87fde766bb610a4b48f40db92e99b177f47[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Apr 6 10:12:18 2018 +0800

    typo

[33mcommit d9d641897fc34993e8d31272ea9765470aff611b[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 21:15:05 2018 +0800

    MethodArea: replace std::vector with std::set

[33mcommit ffccb6b421ba61e5e22e5513c90f0c91f8cc4cfc[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 21:10:55 2018 +0800

    README: upload README.EN.md

[33mcommit 73fefaf6db068db8f959ff3439e6813c68cb6ddd[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 20:13:24 2018 +0800

    README: update usage

[33mcommit fd3fa750aeca4f85c374a386cfad23fb6e17c721[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 20:07:09 2018 +0800

    CommandParser: merge pull request code

[33mcommit c0bcfe4f8641e2a0bf0c7dc934a38df6ad5be4d0[m
Merge: 9930db1 5d11675
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Apr 4 23:01:18 2018 -0500

    Merge pull request #1 from fushang000/master
    
    Create command paser

[33mcommit 5d11675775127e115bae2a0b19d2cf88642ab1c6[m
Author: lxf <1092758202@qq.com>
Date:   Thu Apr 5 11:59:55 2018 +0800

    Create command paser

[33mcommit 9930db1027d7c6f46bcd4756b038ad9324bd5165[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 11:42:26 2018 +0800

    Code clean up

[33mcommit 12032ff4a32b5d81fe5e89b7c73a7a52c049f737[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Apr 5 11:34:57 2018 +0800

    Concurrency: support synchronized(){} code block[BUGS TO FIX]

[33mcommit 67fddd7558bcd9664caab90e1dad04e74e21d657[m
Author: racaljk <1948638989@qq.com>
Date:   Tue Apr 3 14:53:50 2018 +0800

    Concurrency: fix java heap destructor error

[33mcommit 842c5928e6415afd9d085ded7ee622bf64374275[m
Author: racaljk <1948638989@qq.com>
Date:   Tue Apr 3 14:35:14 2018 +0800

    Concurrency: now we can create native thread using new Thread(Runnable r)[ BUG TO FIXUP]

[33mcommit 2b4c884a40d0e5d4f1cfc126d3ed7db5268f7786[m
Author: racaljk <1948638989@qq.com>
Date:   Tue Apr 3 09:03:52 2018 +0800

    JDK: rewrite java.lang.Math::random()

[33mcommit 582d3100512f85577f8a034aa779a0f83fc1fa86[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 22:56:54 2018 +0800

    README: add quicksort demo

[33mcommit d20302f0ce0762be3ef40cb76fb8ba0a1f080074[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 18:58:42 2018 +0800

    JavaException: replace stack trace clear and sweep exception mark into an inline function

[33mcommit 2a4fe80ecc6b9a3b856082e903906c59d901d013[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 18:55:23 2018 +0800

    JavaException: fix exceptio bug

[33mcommit 6981dba4ae3a2955f5d8b9fbdf07d6f4adee6591[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 18:53:16 2018 +0800

    Revert "JavaException: fix exception printing bug"
    
    This reverts commit 1eb46bee01e56321e9a2ccd72b0e0512d9e11fe9.

[33mcommit 1eb46bee01e56321e9a2ccd72b0e0512d9e11fe9[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 18:53:02 2018 +0800

    JavaException: fix exception printing bug

[33mcommit d578bc44081d1c166ac63e7bb684bc06079cb74b[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 18:18:50 2018 +0800

    JavaException: create corresponding java exception class to handle all about exception routines

[33mcommit d6bd19169f5c2034c70ab0d2e97bd834dd28380b[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 17:02:35 2018 +0800

    ExceptionHandle: now we can output calling stack and exception detailed message when an exception occurred

[33mcommit d3898f978e6bba72caa14b1e8f40a210e3a4e12e[m
Author: racaljk <1948638989@qq.com>
Date:   Mon Apr 2 10:56:57 2018 +0800

    CodeExecution: cleanup code

[33mcommit 79a6165a33f10307a9c379bbcf79dd0c41abb1dc[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 22:29:05 2018 +0800

    ExceptionHandle: print exception stack trace if there is no proper catch clause

[33mcommit 2edd47fb568b1134d0036c26870e056215e778c9[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 21:27:15 2018 +0800

    README: add a big picture

[33mcommit 605b6fa29fd2aa6b83a0a02695ff0d2fe1ce7ac9[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 21:03:37 2018 +0800

    JavaHeap : get instance field by name and descriptor

[33mcommit 36c8ea536021ab41a5565055fd4a3a7e3aca8e35[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 20:28:32 2018 +0800

    InvokeMethod: refactor in progress

[33mcommit 8288c61b83e3bb800c8e3b9c0861f4e6cfa89b3d[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 19:56:35 2018 +0800

    CodeExecution: fix exception handling error

[33mcommit 5e38c562f1df02e1d8a19e12d01810f43ca71bbf[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 17:29:20 2018 +0800

    misc: remove temporary files :-)

[33mcommit dfd1b08516344704080deadbcdaeb922bd6e574b[m
Author: racaljk <1948638989@qq.com>
Date:   Sun Apr 1 17:23:21 2018 +0800

    CodeExecution: clean up template

[33mcommit 2f60fb531d7f2c0a25a946584e7b83ed253e138e[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Mar 31 22:01:35 2018 +0800

    CodeExecution: replcae stack.top()&& stack.pop() with an inline function which combained these operations

[33mcommit 5695d3d6bbe8a7577ac14e943435ec96c50d2f6d[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Mar 31 19:56:00 2018 +0800

    RuntimeEnv: remove unused field

[33mcommit 1976b4c8fdfc543712c3c68a0cbd3fd99ef8fb7c[m
Author: racaljk <1948638989@qq.com>
Date:   Sat Mar 31 11:33:41 2018 +0800

    CodeExecution: support exception handling

[33mcommit c6433f892b2b78acac63b4db0274131f65008ba1[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Mar 30 23:09:29 2018 +0800

    CodeExecution : support exception handler

[33mcommit 0794d9f874520ba4e35f9d189a51e1111896fde4[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Mar 30 15:28:02 2018 +0800

    CodeExecution : implement instanceof

[33mcommit 4ab54b96434fbf1353beee20743efd4bd8e93a92[m
Author: racaljk <1948638989@qq.com>
Date:   Fri Mar 30 12:24:29 2018 +0800

    native : support string literal operation such like +,+=

[33mcommit 1e41a6377ed2c75309a8bcdd809a0865e2e63f13[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 22:25:44 2018 +0800

    Test : remove helloworldtest.class

[33mcommit 7d265de1223ccdf19ce6437d2ac5d8edc1fa7e29[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 22:23:06 2018 +0800

    libaray :  upload libraries

[33mcommit ef963959ee33d7dcb5af7b48be4d53db338b02f9[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 20:56:06 2018 +0800

    Option : add comments for debug options

[33mcommit 632345d523b7a89c230759dd997b1c04603b4ed0[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 20:07:32 2018 +0800

    ydk : 添加ydk

[33mcommit afbc822c0c6252a50a0178c333bb04d1082d9a74[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 19:43:59 2018 +0800

    CodeExecution : 修复无符号溢出问题

[33mcommit 1b768233ba47d21bb74679c025c19d37b7bfc054[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 18:42:20 2018 +0800

    update readme.md

[33mcommit 59c2f1e20c4ff238d846030033923a125539c4dc[m
Author: racaljk <1948638989@qq.com>
Date:   Thu Mar 29 18:34:25 2018 +0800

    update!

[33mcommit 04e75b5a9610fd72953192771bb05e37e5c9751e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Oct 4 19:35:45 2017 +0800

    代码执行引擎：修改bugs

[33mcommit fdf4642063b8cab2baf98dadd43660029127b842[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Oct 4 19:30:07 2017 +0800

    代码执行引擎：修改bugs

[33mcommit a58aaf576d10716c34f7308d2935f359b89a8c43[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Oct 4 19:13:54 2017 +0800

    类加载器：修复类加载器对数组类型错误识别

[33mcommit 97703cfe391c27c62097cfa2b4e0204d7ea91998[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 22:13:28 2017 +0800

    native call:移动位置
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 3265aea8f05922a2dc635db942e7d1b09355ae95[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 21:28:48 2017 +0800

    native call：支持本地方法调用
    bug fixed：修复若干bugs
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 38365e6fe6506608d3664ca686a897fd8a3998d2[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 19:48:39 2017 +0800

    代码执行引擎：完成invokevirtual!!!
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit f08d56f8aa23ecb43abad9e908c560f55fafc109[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 16:54:14 2017 +0800

    代码执行引擎：update

[33mcommit 783f0cc0b08901f7443296d0012119807824ff37[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 16:35:28 2017 +0800

    代码执行引擎：完成invokeinterface

[33mcommit 706380f0fe88d48d64903962b87c6cd97c10186c[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 16:28:24 2017 +0800

    代码执行引擎：update

[33mcommit 554bd5a3fcd1d1f19037dbe31c2219865bcac5a6[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 13:25:50 2017 +0800

    logo：update

[33mcommit 44303c686197dc0e6bad20835711f11f7fae08a0[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 13:24:41 2017 +0800

    logo：更新

[33mcommit 123d6f13878bdcc49ee112c101a491249c8a8a9f[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 12:00:56 2017 +0800

    重构：operand

[33mcommit ebdd652efeaadc358428a33193d1428a4e4e2028[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 11:41:15 2017 +0800

    代码执行引擎：完成invokespecial

[33mcommit 4c5d6a88d4e5f95a72a5a33d3cd2359c80312f32[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Oct 2 09:28:35 2017 +0800

    代码执行引擎：完成instanceof checkcast

[33mcommit 84d3a7b0f3f01ef32660b5097654a23d67da807c[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 23:30:59 2017 +0800

    虚拟机：泛型约束
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 1a659db2ed438cefe50581ecdf1b203934565335[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 23:30:22 2017 +0800

    元对象：全部方法添加unused镇压注解
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit c25ab2d4e332105961a0eca01257d845b452ddf8[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 23:19:50 2017 +0800

    代码执行引擎：checkcast half

[33mcommit 6b169ad80db07e1f648218920d89111abed06ca5[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 20:17:29 2017 +0800

    重构：及时重构

[33mcommit 5169222a9083c5d842dd80ba34b7957d9851a1e1[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 19:58:00 2017 +0800

    重构：及时重构

[33mcommit 34350c5f49668c5fbfbb67ec1a4b2eefecac0351[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 19:48:59 2017 +0800

    重构：及时重构

[33mcommit d6c929d2bae011705fa8ec138da0905cb5db4d11[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 19:36:42 2017 +0800

    重构：及时重构

[33mcommit 1f73369e475f51c15c8ebc0ede3684d99f8d8da4[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 19:19:06 2017 +0800

    代码执行引擎：invokespecial half

[33mcommit a70345ee93cea676e7e1bc987ecf590da8e218c8[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 17:17:21 2017 +0800

    代码执行引擎：invokestatic

[33mcommit 8ec4a0d20f029445c75667f99d2837f32bf8dae2[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 16:39:24 2017 +0800

    代码执行引擎：修复bug

[33mcommit fbb5066ea9297facb134688e763dc6c05b645968[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 16:31:33 2017 +0800

    descriptor：完成descriptor解析

[33mcommit af793fe1f28a8704c46432c5d4a728f9280db0c4[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Oct 1 09:10:43 2017 +0800

    代码执行引擎：更新

[33mcommit 3fcd5fd2a044c001faeafd2ff707001280fc856b[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 30 18:48:26 2017 +0800

    readme:更新

[33mcommit f7b2acf1273a55f545a7fed64600e0f49417d8c4[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 30 18:34:23 2017 +0800

    代码执行引擎：invokestatic开发中

[33mcommit 029d2aa6de2c60787b720cc843d72ad96dbe6569[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 30 17:43:06 2017 +0800

    代码执行引擎：跳表完成

[33mcommit 18d22c9d4d11942c32dddc841e520409e0ff7e53[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 30 16:59:33 2017 +0800

    代码执行引擎：完成field操作

[33mcommit 7dd92a597cad6266e92bfdcfbd0ed0f4f8c8a95c[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 22:25:27 2017 +0800

    logo:添加小logo

[33mcommit 1b32f147df995c827e905ef5e518c8eb2d4b3d59[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 22:18:10 2017 +0800

    代码执行引擎：getfield&getstatic完成

[33mcommit 75a2845836c9efe40d764f07218c89c9d5fb2c82[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 19:26:22 2017 +0800

    update logo

[33mcommit f05f204aeaa3be974c5921b4c380f15c93281d90[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 19:23:22 2017 +0800

    logo:上传logo：）

[33mcommit 291bbc65776e3072da83e1cd2ae2f3e057c1b71e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 19:12:16 2017 +0800

    代码执行引擎：multianewarray完成！

[33mcommit b1b9b2744f73da20be00f9322501c2833615dbc6[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 18:32:36 2017 +0800

    代码执行引擎：完成newarray

[33mcommit a40871cf1875622a826b44b31a7bab07363a3091[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 17:58:28 2017 +0800

    元类：修复元类构造函数递归bug

[33mcommit 0aedd9ce89e335b0d6670c99b90e4ffa73dca4f0[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 17:50:12 2017 +0800

    代码执行引擎：完成new

[33mcommit fb5b42bf515d7fef24fecfaa17808f94c7e7d093[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 17:34:43 2017 +0800

    代码执行引擎：完成new

[33mcommit 66884deb55cb5cb93143d07f08a8451c2e014296[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 16:00:29 2017 +0800

    代码执行引擎：完成数组分配

[33mcommit a19fa8148e4649909775159216eea5c7357e3bf7[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 15:26:00 2017 +0800

    代码执行引擎：代码替换完毕

[33mcommit f9577c5ff451c75956ecffa8c161adbe9230fc46[m
Author: Cthulhu <1948638989@qq.com>
Date:   Fri Sep 29 13:43:53 2017 +0800

    元对象：重新设计结构

[33mcommit 8a63b44ade5def122feaa0c4ce5eeec406de7182[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 22:03:36 2017 +0800

    代码执行引擎：athrow done

[33mcommit c7339e5bc997be063a60fd415f449d7cd36b910e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 19:27:31 2017 +0800

    代码执行引擎：areturn working

[33mcommit 42ca4265d4375131e83fd48d332a68c5a00b2849[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 19:10:28 2017 +0800

    代码执行引擎：支持同步方法

[33mcommit 061f7dfd97980e3f82f2fb59262fd8aa9628f4b3[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 17:52:30 2017 +0800

    类加载器：出于效率考虑，去除关联类加载器

[33mcommit 06269de1427ae3b6a160f98245554b4a16dedd63[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 17:50:12 2017 +0800

    类文件读取器：修复文件名验证错误bug

[33mcommit b9c68bf47a1885121637cdf769901573bb00b764[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 10:11:43 2017 +0800

    元类：删除getter setter
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit a3bce803ba802dcf542da06f891e34c4ce3aa2cd[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 10:01:17 2017 +0800

    重构：将线程数据统一由RuntimeThread管理
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 48ae3eb6f82033419c394e8e474778e5afb3fcd6[m
Author: Cthulhu <1948638989@qq.com>
Date:   Thu Sep 28 09:48:51 2017 +0800

    注释：为代码执行引擎添加一些注释
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit ed6997c95d912234cad5f754500c9e0c492fcd82[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 22:41:45 2017 +0800

    简答重构

[33mcommit c69bd5f25bcac9758d50dee94758b02b0aff093f[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 22:01:47 2017 +0800

    代码执行引擎：修复bugs

[33mcommit 9b4fea4b63d5fa149b99efe1c457b17d7c6cbcba[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 21:26:10 2017 +0800

    代码执行引擎：完成所有简单opcodes！

[33mcommit 90db278cddeba5761f195e60a421d74c0097f710[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 20:58:39 2017 +0800

    code execution:update

[33mcommit 89d3595c3d0f676dabaede4baa3cbfc6a3e6a966[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 20:21:35 2017 +0800

    code execution:change vm structure

[33mcommit 605007ef4b658a3339cf7b0fe49615a85641877a[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 20:05:21 2017 +0800

    class loading chain: delete useless codes by adding an parameter at method YMethodScope.existClass

[33mcommit b6fa9d3af45001741fed1826717b56cb55c045de[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 18:57:27 2017 +0800

    code execution: comparison done.

[33mcommit 0c38e63208a9bdff290a02b14d4aeaaa63601432[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 18:42:54 2017 +0800

    code execution: type convert done

[33mcommit b1a547bd2c5ac8171b1155f0f45f4913483149b8[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 12:36:22 2017 +0800

    code execution:update

[33mcommit 85a0bba86fffe519248676e5eaa250bf72c6381a[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 12:34:42 2017 +0800

    code execution: update some opcodes
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit eecd42d049ff1661fbe323220cc867ca63470f4d[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 12:19:45 2017 +0800

    class file reader: support read class paths from config file in /conf/rtsearch.xml

[33mcommit 83f2d530bd33d1f82b7168096f75e9a20fefcb8d[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 11:04:19 2017 +0800

    debug: nicer debug output

[33mcommit 1c6f59f13d49e3b2a7533f9ab7d18f9f509b12c0[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 10:49:37 2017 +0800

    yvm: loading and linking inherit chain before initiating current class

[33mcommit 9b1d0ba6356f5446b66a1ec8989ecc1e34990b26[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 10:40:16 2017 +0800

    yvm: fix bugs

[33mcommit bade7a6efaa0a71301e2d52fb88592a327f66f3e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Wed Sep 27 10:34:14 2017 +0800

    yvm: loading class inherit chain classes

[33mcommit 31cbf826cb075b874c16a210d10d26a5685aebf7[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 22:50:25 2017 +0800

    code execution: update some opcodes
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 2cc8a179b5074aefc94340f163f5acaaa2e0ce4f[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 15:31:00 2017 +0800

    code execution:finish some opcodes

[33mcommit 0ff84d10d976b0f76672506636432d3b00fd05d0[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 14:28:06 2017 +0800

    code execution:finish anewarray

[33mcommit 240e1e8d62e352c99c6b1869a127191aeb51802f[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 12:32:39 2017 +0800

    constant pool: fix float/double resolving bug

[33mcommit 2d8ecdc2ef04d5c0393d57c5b2b3457a2d6c22d7[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 12:06:39 2017 +0800

    constant pool: resolving constant pool in linking

[33mcommit 43e7542361ba5d55d733bdef8d5d67dfe40230c7[m
Author: Cthulhu <1948638989@qq.com>
Date:   Tue Sep 26 09:37:57 2017 +0800

    example: new feature for testing in Example class
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 941ba79df032da0a3cbaa040cc542e8730937960[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 23:50:29 2017 +0800

    class reader:add user class path
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 5d81d5e262d7e5ca33755ee9b02a7fcf5a75015e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 12:42:46 2017 +0800

    runtime structure: add some necessary runtime structure
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit f12c964ed700a0d4c505eaad3c51c595781ec27e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 12:42:46 2017 +0800

    readme: add plans
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 359cd8ed67d6d094bf306fff422a5146cd0492d4[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 12:27:31 2017 +0800

    code execution: Mnemonic.aaload done.
    Mnemonic.aastore done.
    Mnemonic.aconst_null done.
    Mnemonic.aload_* done.
    update
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 9dabed047a516f0c14650ebbb68ad664de7f5eab[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 11:27:33 2017 +0800

    code execution: Mnemonic.aaload done.Mnemonic.aastore done.
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit f112a293c282f8b912da9fc6a03aeee783a9f74d[m
Author: Cthulhu <1948638989@qq.com>
Date:   Mon Sep 25 10:54:11 2017 +0800

    opcode: push stack frame available
    Signed-off-by: Cthulhu <1948638989@qq.com>
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit cdb4c9678a8d119169edf40969045698abba92d5[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Sep 24 13:22:45 2017 +0800

    opcode: add debug method for developing
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 4c5eb66f6d5157bee05226680f9e8d65ffc0ed32[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Sep 24 12:29:02 2017 +0800

    opcode: finish  opcode recognition
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 0b6dc9aa8c384da3f2243812a89beec7d12124e6[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sun Sep 24 12:15:45 2017 +0800

    opcode: finish lookupswitch opcode recognition
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 82aa1314b5397544b3ae080d1a5c0b5f5b4d7f77[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 23 23:30:58 2017 +0800

    opcode: finish opcode recognition
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 4351e4aa1c1ff4782b304659632fabe1c78da07e[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 23 18:52:21 2017 +0800

    readme:update project summary and license note
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit 2dba1f7c610afba2c3b8c316f74deb04460dfb0f[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 23 18:43:10 2017 +0800

    init:Initial commit
    
    Signed-off-by: Cthulhu <1948638989@qq.com>

[33mcommit da681184b0ba0a87d57f644d41f16758ab54a31c[m
Author: Cthulhu <1948638989@qq.com>
Date:   Sat Sep 23 18:36:00 2017 +0800

    Initial commit
