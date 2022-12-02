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
