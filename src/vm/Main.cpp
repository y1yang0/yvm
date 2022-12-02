#include <iostream>
#include <sstream>
#include "../misc/Option.h"
#include "YVM.h"

int main(int argc, char* argv[]) {
    if (argc != 3 || strstr(argv[1], "--lib") == NULL) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  yvm --lib=<path> <main_class>" << std::endl;
        std::cout << std::endl;
        std::cout << "      --lib=<path>     Tells YVM where to find JDK classes(java.lang.String, etc)" << std::endl;
        std::cout << "      <main_class>     The full qualified Java class name, e.g. org.example.Foo" << std::endl;
        return 0;
    }

    YVM yvm;
    std::string libs = argv[1] + strlen("--lib=");
    yvm.initialize(libs);
    std::string mainClass = argv[2];
    for (auto& c : mainClass) {
        if (c == '.') {
            c = '/';
        }
    }
    yvm.callMain(mainClass);
    return 0;
}