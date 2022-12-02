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

#ifndef YVM_JAVAOBJECT_H
#define YVM_JAVAOBJECT_H

#include <cstddef>
#include <cstdint>

#define BASE_OF_JTYPE :public JType

class JavaClass;

struct JType {
    virtual ~JType() = default;  // reserved for polymorphism
};

using JRef = JType;

struct JVoid BASE_OF_JTYPE {};

struct JDouble BASE_OF_JTYPE {
    explicit JDouble() = default;
    explicit JDouble(double val) : val(val) {}
    double val = 0.0;
};

struct JFloat BASE_OF_JTYPE {
    explicit JFloat() = default;
    explicit JFloat(float val) : val(val) {}
    float val = 0.0f;
};

struct JInt BASE_OF_JTYPE {
    explicit JInt() = default;
    explicit JInt(int32_t val) : val(val) {}
    int32_t val = 0;
};

struct JLong BASE_OF_JTYPE {
    explicit JLong() = default;
    explicit JLong(int64_t val) : val(val) {}
    int64_t val = 0L;
};

struct JObject BASE_OF_JTYPE {
    explicit JObject() = default;

    std::size_t offset = 0;  // Offset on java heap
    const JavaClass* jc{};   // Reference to meta java class
};

struct JArray BASE_OF_JTYPE {
    explicit JArray() = default;

    int length = 0;          // Length of java array
    std::size_t offset = 0;  // Offset on java heap
};

#define IS_JINT(x) (typeid(*x) == typeid(JInt))
#define IS_JLong(x) (typeid(*x) == typeid(JLong))
#define IS_JDouble(x) (typeid(*x) == typeid(JDouble))
#define IS_JFloat(x) (typeid(*x) == typeid(JFloat))
#define IS_JObject(x) (typeid(*x) == typeid(JObject))
#define IS_JArray(x) (typeid(*x) == typeid(JArray))

#endif  // !YVM_OBJECT_H
