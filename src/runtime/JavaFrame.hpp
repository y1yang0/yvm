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

#ifndef YVM_JAVAFRAME_H
#define YVM_JAVAFRAME_H

#include <exception>
#include <list>
#include "../gc/Concurrent.hpp"
#include "../interpreter/Internal.h"
#include "../misc/Utils.h"
#include "JavaType.h"

using namespace std;

struct JType;

class Slots {
    friend class JavaFrame;
    friend class ConcurrentGC;
    friend class Interpreter;

public:
    explicit Slots(int maxLocal, int maxStack);

    ~Slots();

    // Check if current frame's stack slots were empty
    bool emptyStack() const { return stackTop == 0; }

    // Load variable from local variable by given index and push it to stack
    template <typename LoadType>
    void load(u1 localIndex);

    // Store variable from stack top to local variable by given index and
    // pop it from stack
    template <typename StoreType>
    void store(u1 index);

    // Push new variable to current frame's stack slot
    void push(JType *var) { stackSlots[stackTop++] = var; }

    // Pop variable from top of the current frame's stack
    template <typename PopType>
    PopType *pop();

    // Set new variable to current frame's local variable slot
    void setLocalVariable(u1 index, JType *var);

    // Get variable from local variables by given index
    JType *getLocalVariable(u1 index) { return localSlots[index]; }

    // Dump current frame to stdout
    void dump();

    // Reallocate stack slots size accorrding to argument, this can be used when
    // an exception occurred
    void grow(int size);

private:
    std::list<JType *> exceptions;
    JType **localSlots;
    JType **stackSlots;
    const int maxLocal;
    int maxStack;
    int stackTop;
    Slots *next;
};

//--------------------------------------------------------------------------------
// Java frame represents runtime frames. Each interpreter has a JavaFrame, it
// consists of many Slots, one Slots is logically divided into local variable
// slots and stack slots
//--------------------------------------------------------------------------------
class JavaFrame {
    friend class ConcurrentGC;

public:
    ~JavaFrame();

    // Push new frame
    void pushFrame(int maxLocal, int maxStack);

    // Pop top frame
    void popFrame();

    // Check if there is more frames
    bool hasFrame() const { return top_ != nullptr; }

    // Return top frame
    Slots *top() { return top_; }

    // Return next to top's frame
    Slots *nextFrame() { return top_->next; }

private:
    Slots *top_{};
};

template <typename LoadType>
inline void Slots::load(u1 localIndex) {
    auto *var = new LoadType;
    var->val = dynamic_cast<LoadType *>(localSlots[localIndex])->val;
    stackSlots[stackTop++] = var;
}

template <>
inline void Slots::load<JRef>(u1 localIndex) {
    JType *var{};
    if (typeid(*localSlots[localIndex]) == typeid(JObject)) {
        var = new JObject;
        dynamic_cast<JObject *>(var)->jc =
            dynamic_cast<JObject *>(localSlots[localIndex])->jc;
        dynamic_cast<JObject *>(var)->offset =
            dynamic_cast<JObject *>(localSlots[localIndex])->offset;
    } else if (typeid(*localSlots[localIndex]) == typeid(JArray)) {
        var = new JArray;
        dynamic_cast<JArray *>(var)->length =
            dynamic_cast<JArray *>(localSlots[localIndex])->length;
        dynamic_cast<JArray *>(var)->offset =
            dynamic_cast<JArray *>(localSlots[localIndex])->offset;
    } else {
        SHOULD_NOT_REACH_HERE
    }
    stackSlots[stackTop++] = var;
}

template <typename StoreType>
inline void Slots::store(u1 localIndex) {
    if (stackTop) {
        stackTop--;
    }
    auto *var = dynamic_cast<StoreType *>(stackSlots[stackTop]);
    stackSlots[stackTop] = nullptr;
    localSlots[localIndex] = var;
    if (IS_JLong(var) || IS_JDouble(var)) {
        localSlots[localIndex++] = nullptr;
    }
}

template <typename PopType>
inline PopType *Slots::pop() {
    if (stackTop) {
        stackTop--;
    }
    auto *var = dynamic_cast<PopType *>(stackSlots[stackTop]);
    stackSlots[stackTop] = nullptr;
    return var;
}

#endif
