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

#ifndef _METHODRESOLVE_H
#include <map>
#include <string>
#include "../classfile/ClassFile.h"
#include "../runtime/JavaClass.h"
#include "CallSite.h"

//--------------------------------------------------------------------------------
// If C contains a declaration for an instance method m that overrides the
// resolved method, then m is the method to be invoked.
//--------------------------------------------------------------------------------
CallSite findInstanceMethod(const JavaClass* jc, const std::string& methodName,
                            const std::string& methodDescriptor);

//--------------------------------------------------------------------------------
// If C has a superclass, a search for a declaration of an instance
// method that overrides the resolved method is performed, starting with the
// direct superclass of C and continuing with the direct superclass of that
// class, and so forth, until an overriding method is found or no further
// superclasses exist.If an overriding method is found, it is the method to be
// invoked.
//--------------------------------------------------------------------------------
CallSite findInstanceMethodOnSupers(const JavaClass* jc,
                                    const std::string& methodName,
                                    const std::string& methodDescriptor);

//--------------------------------------------------------------------------------
// If C is an interface and the class Object contains a declaration of a public
// instance method with the same name and descriptor as the resolved method,
// then it is the method to be invoked.
//--------------------------------------------------------------------------------
CallSite findMaximallySpecifiedMethod(const JavaClass* jc,
                                      const std::string& methodName,
                                      const std::string& methodDescriptor);

//--------------------------------------------------------------------------------
// If there is exactly one maximally - specific method(5.4.3.3) in the
// superinterfaces of C that matches the resolved method's name and descriptor
// and is not abstract, then it is the method to be invoked.
//--------------------------------------------------------------------------------
CallSite findJavaLangObjectMethod(const JavaClass* jc,
                                  const std::string& methodName,
                                  const std::string& methodDescriptor);

#endif  // !_METHODRESOLVE_H
