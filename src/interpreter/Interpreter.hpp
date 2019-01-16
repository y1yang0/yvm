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
extern RuntimeEnv yrt;

class Interpreter {
public:
    explicit Interpreter() : frames(new JavaFrame) {}

    explicit Interpreter(JavaFrame* frames) : frames(frames) {}

    ~Interpreter();

    void invokeByName(JavaClass* jc, const std::string& methodName,
                      const std::string& methodDescriptor);
    void invokeInterface(const JavaClass* jc, const std::string& methodName,
                         const std::string& methodDescriptor);
    void invokeSpecial(const JavaClass* jc, const std::string& methodName,
                       const std::string& methodDescriptor);
    void invokeStatic(const JavaClass* jc, const std::string& methodName,
                      const std::string& methodDescriptor);
    void invokeVirtual(const std::string& methodName,
                       const std::string& methodDescriptor);

private:
    JType* getStaticField(JavaClass* parsedJc, const std::string& fieldName,
                          const std::string& fieldDescriptor);
    void putStaticField(JavaClass* parsedJc, const std::string& fieldName,
                        const std::string& fieldDescriptor, JType* value);

    bool checkInstanceof(const JavaClass* jc, u2 index, JType* objectref);

    std::tuple<JavaClass*, std::string, std::string>
    parseFieldSymbolicReference(const JavaClass* jc, u2 index) const;
    std::tuple<JavaClass*, std::string, std::string>
    parseInterfaceMethodSymbolicReference(const JavaClass* jc, u2 index) const;
    std::tuple<JavaClass*, std::string, std::string>
    parseMethodSymbolicReference(const JavaClass* jc, u2 index) const;
    std::tuple<JavaClass*> parseClassSymbolicReference(const JavaClass* jc,
                                                       u2 index) const;

    JObject* execNew(const JavaClass* jc, u2 index);
    JType* execByteCode(const JavaClass* jc, u1* code, u4 codeLength,
                        u2 exceptLen, ExceptionTable* exceptTab);
    JType* execNativeMethod(const std::string& className,
                            const std::string& methodName,
                            const std::string& methodDescriptor);

    void loadConstantPoolItem2Stack(const JavaClass* jc, u2 index);

    bool handleException(const JavaClass* jc, u2 exceptLen,
                         ExceptionTable* exceptTab, const JObject* objectref,
                         u4& op);

    void pushMethodArguments(std::vector<int>& parameter, bool isObjectMethod);

private:
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
    delete value2;
    delete value1;
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
    delete value;
}

#endif  // YVM_INTERPRETER_H
