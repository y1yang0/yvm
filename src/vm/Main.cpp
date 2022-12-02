// MIT License
//
// Copyright (c) 2017 Yi Yang <kelthuzadx@qq.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <iostream>
#include <sstream>
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

    std::string libs = argv[1] + strlen("--lib=");
    YVM::initialize(libs);
    std::string mainClass = argv[2];
    for (auto& c : mainClass) {
        if (c == '.') {
            c = '/';
        }
    }
    YVM::callMain(mainClass);
    return 0;
}