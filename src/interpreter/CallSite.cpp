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

#include "CallSite.h"

CallSite::CallSite()
    : jc(nullptr), code(nullptr), exception(nullptr), callable(false) {}

CallSite CallSite::makeCallSite(const JavaClass* jc, MethodInfo* m) {
    CallSite cs;
    cs.callable = m != nullptr ? true : false;
    cs.accessFlags = m->accessFlags;
    cs.jc = jc;

    FOR_EACH(i, m->attributeCount) {
        if (typeid(*m->attributes[i]) == typeid(ATTR_Code)) {
            cs.code = dynamic_cast<ATTR_Code*>(m->attributes[i])->code;
            cs.codeLength = ((ATTR_Code*)m->attributes[i])->codeLength;
            cs.maxLocal = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxLocals;
            cs.maxStack = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxStack;
            cs.exceptionLen = dynamic_cast<ATTR_Code*>(m->attributes[i])
                                  ->exceptionTableLength;
            cs.exception =
                dynamic_cast<ATTR_Code*>(m->attributes[i])->exceptionTable;
            break;
        }
    }
    return cs;
}
