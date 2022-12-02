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

#include "SymbolicRef.h"

SymbolicRef parseFieldSymbolicReference(const JavaClass *jc, u2 index) {
    auto *fr = (CONSTANT_Fieldref *)jc->getConstPoolItem(index);
    auto *nat =
        (CONSTANT_NameAndType *)jc->getConstPoolItem(fr->nameAndTypeIndex);
    auto *cl = (CONSTANT_Class *)jc->getConstPoolItem(fr->classIndex);

    auto fieldName = jc->getString(nat->nameIndex);
    auto fieldDesc = jc->getString(nat->descriptorIndex);
    auto fieldClass =
        runtime.cs->loadClassIfAbsent(jc->getString(cl->nameIndex));
    runtime.cs->linkClassIfAbsent(jc->getString(cl->nameIndex));

    return SymbolicRef{fieldClass, fieldName, fieldDesc};
}

SymbolicRef parseInterfaceMethodSymbolicReference(const JavaClass *jc,
                                                  u2 index) {
    auto *imr = (CONSTANT_InterfaceMethodref *)jc->getConstPoolItem(index);
    auto *nat =
        (CONSTANT_NameAndType *)jc->getConstPoolItem(imr->nameAndTypeIndex);
    auto *cl = (CONSTANT_Class *)jc->getConstPoolItem(imr->classIndex);

    auto interfaceMethodName = jc->getString(nat->nameIndex);
    auto interfaceMethodDesc = jc->getString(nat->descriptorIndex);
    auto interfaceMethodClass =
        runtime.cs->loadClassIfAbsent(jc->getString(cl->nameIndex));
    runtime.cs->linkClassIfAbsent(jc->getString(cl->nameIndex));

    return SymbolicRef{interfaceMethodClass, interfaceMethodName,
                       interfaceMethodDesc};
}

SymbolicRef parseMethodSymbolicReference(const JavaClass *jc, u2 index) {
    auto *mr = (CONSTANT_Methodref *)jc->getConstPoolItem(index);
    auto *nat =
        (CONSTANT_NameAndType *)jc->getConstPoolItem(mr->nameAndTypeIndex);
    auto *cl = (CONSTANT_Class *)jc->getConstPoolItem(mr->classIndex);

    auto methodName = jc->getString(nat->nameIndex);
    auto methodDesc = jc->getString(nat->descriptorIndex);
    auto methodClass =
        runtime.cs->loadClassIfAbsent(jc->getString(cl->nameIndex));
    runtime.cs->linkClassIfAbsent(jc->getString(cl->nameIndex));

    return SymbolicRef{methodClass, methodName, methodDesc};
}

SymbolicRef parseClassSymbolicReference(const JavaClass *jc, u2 index) {
    auto *cl = (CONSTANT_Class *)jc->getConstPoolItem(index);
    auto className = jc->getString(cl->nameIndex);
    if (className[0] == '[') {
        className = peelArrayComponentTypeFrom(className);
    }

    auto c = runtime.cs->loadClassIfAbsent(className);
    runtime.cs->linkClassIfAbsent(className);
    return SymbolicRef{c};
}
