#include "YVM.h"

int main(int argc, const char * argv[]) {
    const char* ydk[] = {
        R"(C:\Users\Cthulhu\Desktop\yvm\javalib)",
       // R"(C:\Users\Cthulhu\Desktop\code_warehouse_hp\ydktest\out\production\ydktest)"
    };

    YVM vm;
    vm.warmUp(ydk, sizeof(ydk) / sizeof(ydk[0]));
    vm.callMain("ydk/test/InheritanceTest");
    vm.callMain("ydk/test/HelloWorldTest");
    vm.callMain("ydk/test/BubbleSortTest");
    vm.callMain("ydk/test/StringConcatenation");
    vm.callMain("ydk/test/InstanceofTest");
    //vm.callMain("ydk/test/ThrowExceptionTest");
    //vm.callMain("ydk/test/StackTraceTest");
    vm.callMain("ydk/test/QuickSort");
    vm.callMain("ydk/test/MathTest");
    vm.callMain("ydk/test/CreateAsyncThreadsTest");
    system("pause");
    return 0;
}