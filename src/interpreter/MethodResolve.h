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

std::tuple<JavaClass*, std::string, std::string> parseFieldSymbolicReference(
    const JavaClass* jc, u2 index);

std::tuple<JavaClass*, std::string, std::string>
parseInterfaceMethodSymbolicReference(const JavaClass* jc, u2 index);

std::tuple<JavaClass*, std::string, std::string> parseMethodSymbolicReference(
    const JavaClass* jc, u2 index);

std::tuple<JavaClass*> parseClassSymbolicReference(const JavaClass* jc,
                                                   u2 index);

#endif  // !_METHODRESOLVE_H
