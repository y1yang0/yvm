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

#ifndef YVM_INTERPRETER_H
#define YVM_INTERPRETER_H

#include <typeinfo>
#include "../classfile/ClassFile.h"
#include "../runtime/JavaException.h"
#include "../runtime/JavaFrame.hpp"
#include "../runtime/JavaHeap.hpp"
#include "../runtime/JavaType.h"
#include "../runtime/RuntimeEnv.h"

#pragma warning(disable : 4244)

struct MethodInfo;
struct RuntimeEnv;
extern RuntimeEnv runtime;
using std::string;
class Interpreter {
public:
    explicit Interpreter() : frames(new JavaFrame) {}

    explicit Interpreter(JavaFrame* frames) : frames(frames) {}

    ~Interpreter();

    void invokeByName(JavaClass* jc, const string& name,
                      const string& descriptor);
    void invokeInterface(const JavaClass* jc, const string& name,
                         const string& descriptor);
    void invokeSpecial(const JavaClass* jc, const string& name,
                       const string& descriptor);
    void invokeStatic(const JavaClass* jc, const string& name,
                      const string& descriptor);
    void invokeVirtual(const string& name, const string& descriptor);

private:
    bool checkInstanceof(const JavaClass* jc, u2 index, JType* objectref);

    JObject* execNew(const JavaClass* jc, u2 index);
    JType* execByteCode(const JavaClass* jc, u1* code, u4 codeLength,
                        u2 exceptLen, ExceptionTable* exceptTab);
    JType* execNativeMethod(const string& className, const string& methodName,
                            const string& methodDescriptor);

    void loadConstantPoolItem2Stack(const JavaClass* jc, u2 index);

    bool handleException(const JavaClass* jc, u2 exceptLen,
                         ExceptionTable* exceptTab, const JObject* objectref,
                         u4& op);

    void pushMethodArguments(std::vector<int>& parameter, bool isObjectMethod);

private:
    template <typename ResultType>
    void fmodArithmetic() const;

    template <typename ResultType, typename CallableObjectType>
    void binaryArithmetic(CallableObjectType op);

    template <typename ResultType, typename CallableObjectType>
    void unaryArithmetic(CallableObjectType op);

    template <typename Type1, typename Type2>
    void typeCast() const;

private:
    JavaFrame* frames;
    JavaException exception;
};

template <typename ResultType, typename CallableObjectType>
void Interpreter::binaryArithmetic(CallableObjectType op) {
    auto* value2 = frames->top()->pop<ResultType>();
    auto* value1 = frames->top()->pop<ResultType>();
    auto* result = new ResultType;
    result->val = op(value1->val, value2->val);
    frames->top()->push(result);
}

template <typename ResultType, typename CallableObjectType>
void Interpreter::unaryArithmetic(CallableObjectType op) {
    auto* ival = frames->top()->pop<ResultType>();
    ival->val = op(ival->val);
    frames->top()->push(ival);
}

template <typename Type1, typename Type2>
void Interpreter::typeCast() const {
    auto* value = frames->top()->pop<Type1>();
    auto* result = new Type2;
    result->val = value->val;
    frames->top()->push(result);
}

template <typename ResultType>
void Interpreter::fmodArithmetic() const {
    auto* value2 = frames->top()->pop<ResultType>();
    auto* value1 = frames->top()->pop<ResultType>();
    auto* result = new ResultType;
    result->val = std::fmod(value1->val, value2->val);
    frames->top()->push(result);
}

#endif  // YVM_INTERPRETER_H
