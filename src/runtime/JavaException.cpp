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

#include <cassert>
#include <cstdio>
#include "JavaClass.h"
#include "JavaException.h"

void StackTrace::printStackTrace() {
    assert(!exceptionStackTrace.empty());
    assert(throwExceptionClass != nullptr);

    printf("Thrown %s at %s()\n", throwExceptionClass->getClassName().c_str(),
           exceptionStackTrace[0].c_str());
    !detailedMsg.empty() ? printf("Reason:%s\n", detailedMsg.c_str())
                         : printf("");

    int deep = 0;
    for (size_t p = 1; p < exceptionStackTrace.size(); p++) {
        deep++;
        for (int i = 0; i < deep; i++) {
            printf("-");
        }
        printf("By its caller %s()\n", exceptionStackTrace[p].c_str());
    }
}

void StackTrace::setThrowExceptionInfo(JObject* throwableObject) {
    throwExceptionClass = throwableObject->jc;
    auto* messageField = dynamic_cast<JObject*>(runtime.heap->getFieldByName(
        runtime.cs->findJavaClass("java/lang/Throwable"), "message",
        "Ljava/lang/String;", throwableObject));
    detailedMsg = javastring2stdtring(messageField);
}
