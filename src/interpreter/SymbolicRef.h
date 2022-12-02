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

#ifndef _SYMBOLIC_REFERENCE_H
#define _SYMBOLIC_REFERENCE_H

#include <string>
#include "../runtime/JavaClass.h"

struct SymbolicRef {
    explicit SymbolicRef() : jc(nullptr) {}
    explicit SymbolicRef(JavaClass* jc) : jc(jc) {}
    explicit SymbolicRef(JavaClass* jc, std::string name,
                         std::string descriptor)
        : jc(jc), name(name), descriptor(descriptor) {}

    JavaClass* jc;
    std::string name;
    std::string descriptor;
};

SymbolicRef parseFieldSymbolicReference(const JavaClass* jc, u2 index);

SymbolicRef parseInterfaceMethodSymbolicReference(const JavaClass* jc,
                                                  u2 index);

SymbolicRef parseMethodSymbolicReference(const JavaClass* jc, u2 index);

SymbolicRef parseClassSymbolicReference(const JavaClass* jc, u2 index);

#endif
