#include "YVM.h"

int main() {
    const char * ydk[] = {
        R"(C:\Users\Cthulhu\Desktop\yvm\library)"
    };

    YVM vm;
    vm.warmUp(ydk, sizeof(ydk) / sizeof(ydk[0]));
    vm.callMain("ydk/test/InheritanceTest");
    system("pause");
    return 0;
}