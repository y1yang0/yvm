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

#ifndef YVM_EXCEPTION_H
#define YVM_EXCEPTION_H
#include <string>
#include <vector>
#include "JavaType.h"

class JavaClass;

class StackTrace {
public:
    void printStackTrace();
    void setThrowExceptionInfo(JObject* throwableObject);
    void extendExceptionStackTrace(const std::string& methodName) {
        exceptionStackTrace.push_back(methodName);
    }

protected:
    std::vector<std::string> exceptionStackTrace;

private:
    const JavaClass* throwExceptionClass = nullptr;
    std::string detailedMsg;
};

class JavaException : public StackTrace {
public:
    bool hasUnhandledException() const { return unhandledException; }
    void markException() { unhandledException = true; }
    void sweepException() {
        unhandledException = false;
        exceptionStackTrace.clear();
    }

private:
    volatile bool unhandledException = false;
};

#endif
