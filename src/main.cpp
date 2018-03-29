#include "YVM.h"


int main() {
    const char * ydk[] = {
        R"(C:\Users\Cthulhu\Desktop\code_warehouse_hp\java\lang\BasicLang\out\production\lang)",
        R"(C:\Users\Cthulhu\Desktop\searchpath)"
    };

    YVM vm;
    vm.warmUp(ydk, sizeof(ydk) / sizeof(ydk[0]));
    vm.callMain("ydk/test/BubbleSortTest");
    system("pause");
    return 0;
}