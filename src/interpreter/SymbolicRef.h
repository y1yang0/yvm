#ifndef _SYMBOLIC_REFERENCE_H
#define _SYMBOLIC_REFERENCE_H

#include <string>
#include "../runtime/JavaClass.h"

struct SymbolicRef {
    explicit SymbolicRef() : jc(nullptr) {}
    explicit SymbolicRef(JavaClass* jc) : jc(jc) {}
    explicit SymbolicRef(JavaClass* jc, std::string name,
                         std::string descriptor)
        : jc(jc), name(name), descriptor(descriptor) {}

    JavaClass* jc;
    std::string name;
    std::string descriptor;
};

SymbolicRef parseFieldSymbolicReference(const JavaClass* jc, u2 index);

SymbolicRef parseInterfaceMethodSymbolicReference(const JavaClass* jc,
                                                  u2 index);

SymbolicRef parseMethodSymbolicReference(const JavaClass* jc, u2 index);

SymbolicRef parseClassSymbolicReference(const JavaClass* jc, u2 index);

#endif
