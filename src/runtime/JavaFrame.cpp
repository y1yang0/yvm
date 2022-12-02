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

#include <cstring>
#include "JavaFrame.hpp"

JavaFrame::~JavaFrame() {
    while (top_) {
        auto* temp = top_;
        top_ = top_->next;
        delete temp;
    }
}

void JavaFrame::pushFrame(int maxLocal, int maxStack) {
    auto* slots = new Slots{maxLocal, maxStack};
    slots->next = top_;
    top_ = slots;
}

void JavaFrame::popFrame() {
    auto* temp = top_;
    top_ = top_->next;
    delete temp;
}

Slots::Slots(int maxLocal, int maxStack)
    : maxLocal(maxLocal),
      maxStack(maxStack),
      next(nullptr),
      localSlots(nullptr),
      stackSlots(nullptr) {
    if (maxLocal > 0) {
        localSlots = new JType*[maxLocal];
        memset(localSlots, 0, sizeof(JType*) * maxLocal);
    }
    if (maxStack > 0) {
        stackSlots = new JType*[maxStack];
        memset(stackSlots, 0, sizeof(JType*) * maxStack);
    }

    stackTop = 0;
}

Slots::~Slots() {
    delete[] stackSlots;

    delete[] localSlots;
}

void Slots::setLocalVariable(u1 index, JType* var) {
    if (index > maxLocal) {
        throw std::logic_error("invalid local variable slot index");
    }
    localSlots[index] = var;
}

void Slots::dump() {
    auto* temp = next;
    while (temp != nullptr) {
        printf("stack slots:%d, local variables:%d\n", temp->stackTop,
               maxLocal);
        temp = temp->next;
    }
}

void Slots::grow(int size) {
    JType** newStack = new JType*[size + maxStack];
    memset(newStack, 0, sizeof(JType*) * (size + maxStack));
    for (int i = 0; i < maxStack; i++) {
        newStack[i] = stackSlots[i];
    }
    delete[] stackSlots;
    this->stackSlots = newStack;
    this->maxStack = maxStack + size;
}
