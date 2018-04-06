#ifndef YVM_JAVAFRAME_H
#define YVM_JAVAFRAME_H

#include "JavaType.h"
#include <stack>
#include <deque>

struct JType;

struct Frame {
    std::deque<JType*> locals;
    std::stack<JType*> stack;

    ~Frame() {
        while (!locals.empty()){
            auto* temp = locals.back();
            locals.pop_back();
            delete temp;
        }
        while (!stack.empty()) {
            auto* type = stack.top();
            stack.pop();
            delete type;
        }
    }
};

typedef std::stack<Frame*> StackFrames;

#endif //YVM_JAVAFRAME_H
