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
        for (auto* p : locals) {
            delete p;
        }
        while (!stack.empty()) {
            JType* type = stack.top();
            stack.pop();
            delete type;
        }
    }
};

#endif //YVM_JAVAFRAME_H
