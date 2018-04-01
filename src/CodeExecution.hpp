#ifndef YVM_CODEEXECUTION_H
#define YVM_CODEEXECUTION_H

#include "JavaHeap.h"
#include "RuntimeEnv.h"
#include "JavaType.h"
#include "ClassFile.h"
#include "Frame.h"
#include <typeinfo>

struct MethodInfo;
struct RuntimeEnv;
extern RuntimeEnv yrt;

struct CodeAttrCore {
    bool valid = false;
    u1* code{};
    u4 codeLength{};
    u2 maxStack{};
    u2 maxLocal{};
    u2 exceptionTableLength{};
    ATTR_Code::_exceptionTable* exceptionTable{};
};

/**
* \brief This class does actual bytecode interruption. To accomplish code execution, it needs
* sorts of information with regard to class file structure, runtime heap structure and function
* calling stack,etc. So we set it as a friend class of that classes for convenience.
*/
class CodeExecution {
public:
    CodeExecution() : currentFrame(nullptr) {}

    ~CodeExecution() { currentFrame = nullptr; }

    void invokeByName(JavaClass* jc, const char* methodName, const char* methodDescriptor);
    void invokeInterface(const JavaClass* jc, const char* methodName, const char* methodDescriptor);
    void invokeSpecial(const JavaClass* jc, const char* methodName, const char* methodDescriptor);
    void invokeStatic(const JavaClass* jc, const char* methodName, const char* methodDescriptor);
    void invokeVirtual(const char* methodName, const char* methodDescriptor);
    JType* invokeNative(const char * className, const char * methodName, const char * methodDescriptor);

private:
    std::pair<MethodInfo *, const JavaClass*> findMethod(const JavaClass* jc, const char* methodName,
        const char* methodDescriptor);
    JType* getStaticField(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor);
    void putStaticField(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor, JType* value);

    CodeAttrCore getCodeAttrCore(const MethodInfo* m);
    bool checkInstanceof(const JavaClass * jc, u2 index, JType* objectref);

    std::tuple<JavaClass*, const char*, const char*> parseFieldSymbolicReference(const JavaClass* jc, u2 index);
    std::tuple<JavaClass*, const char*, const char*> parseInterfaceMethodSymbolicReference(
        const JavaClass* jc, u2 index);
    std::tuple<JavaClass*, const char*, const char*> parseMethodSymbolicReference(const JavaClass* jc, u2 index);

    JObject* execNew(const JavaClass* jc, u2 index);
    JType* execCode(const JavaClass* jc, CodeAttrCore && ext);

    void loadConstantPoolItem2Stack(const JavaClass *jc, u2 index);

    bool handleException(const JavaClass * jc, const CodeAttrCore & ext, const JObject * objectref, u4 & op);

    void pushMethodArguments(Frame* frame, std::vector<int>& parameter);

    JObject* pushMethodThisArgument(Frame * frame);

private:
    static inline u2 consumeU2(const u1* code, u4& opidx) {
        const u1 indexbyte1 = code[++opidx];
        const u1 indexbyte2 = code[++opidx];
        const u2 index = (indexbyte1 << 8) | indexbyte2;
        return index;
    }

    static inline u4 consumeU4(const u1* code, u4& opidx) {
        const u1 byte1 = code[++opidx];
        const u1 byte2 = code[++opidx];
        const u1 byte3 = code[++opidx];
        const u1 byte4 = code[++opidx];
        const u4 res = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | (byte4);
        return res;
    }

    inline void popFrame() {
        Frame* f = yrt.frames.top();
        yrt.frames.pop();
        delete f;
        if (!yrt.frames.empty()) {
            currentFrame = yrt.frames.top();
        }
    }

private:
    template <typename LoadType>
    void load2Stack(u1 localIndex);
    template <typename LoadType>
    void loadArrayItem2Stack();
    template <typename StoreType>
    void store2Local(u1 index);
    template <typename StoreType>
    void storeArrayItem();
    template <typename ReturnType>
    ReturnType* flowReturn() const;
    template <typename ReturnType>
    ReturnType* currentStackPop();

    template <typename Type1, typename Type2>
    void typeCast() const;

private:
    inline void extendExceptionStackTrace(const char * methodName) { exceptionStackTrace.push_back(methodName); }
    inline void clearExceptionStackTrace() { exceptionStackTrace.clear(); }

private:
    Frame * currentFrame{};
    std::vector<const char *> exceptionStackTrace;
    bool exceptionUnhandled = false;
};

template <typename LoadType>
inline void CodeExecution::load2Stack(u1 localIndex) {
    auto* pushingV = new LoadType;
    pushingV->val = dynamic_cast<LoadType*>(currentFrame->locals[localIndex])->val;
    currentFrame->stack.push(pushingV);
}

template <>
inline void CodeExecution::load2Stack<JRef>(u1 localIndex) {
    JType* pushingV{};
    if (typeid(*currentFrame->locals[localIndex]) == typeid(JObject)) {
        pushingV = new JObject;
        dynamic_cast<JObject*>(pushingV)->jc = dynamic_cast<JObject*>(currentFrame->locals[localIndex])->jc;
        dynamic_cast<JObject*>(pushingV)->offset = dynamic_cast<JObject*>(currentFrame->locals[localIndex])->offset;
    }
    else if (typeid(*currentFrame->locals[localIndex]) == typeid(JArray)) {
        pushingV = new JArray;
        dynamic_cast<JArray*>(pushingV)->length = dynamic_cast<JArray*>(currentFrame->locals[localIndex])->length;
        dynamic_cast<JArray*>(pushingV)->offset = dynamic_cast<JArray*>(currentFrame->locals[localIndex])->offset;
    }
    else {
        SHOULD_NOT_REACH_HERE
    }
    currentFrame->stack.push(pushingV);
}

template <typename LoadType>
void CodeExecution::loadArrayItem2Stack() {
    JInt* index = (JInt*)currentFrame->stack.top();
    currentFrame->stack.pop();
    auto* arrayref = (JArray*)currentFrame->stack.top();
    currentFrame->stack.pop();
    if (arrayref == nullptr) {
        throw std::runtime_error("null pointer");
    }
    if (index->val > arrayref->length || index->val < 0) {
        throw std::runtime_error("array index out of bounds");
    }
    auto* ival = new LoadType;
    ival->val = dynamic_cast<LoadType*>(yrt.jheap->getArrayItem(*arrayref, index->val))->val;
    currentFrame->stack.push(ival);

    delete index;
    delete arrayref;
}

template <>
inline void CodeExecution::loadArrayItem2Stack<JRef>() {
    auto* index = dynamic_cast<JInt*>(currentFrame->stack.top());

    currentFrame->stack.pop();
    auto* arrayref = dynamic_cast<JArray*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    if (arrayref == nullptr) {
        throw std::runtime_error("null pointer");
    }
    if (index->val > arrayref->length || index->val < 0) {
        throw std::runtime_error("array index out of bounds");
    }

    JType* objectref{};
    JType* ptrArrItem = yrt.jheap->getArrayItem(*arrayref, index->val);
    if (typeid(*ptrArrItem) == typeid(JArray)) {
        objectref = new JArray;
        dynamic_cast<JArray*>(objectref)->length = dynamic_cast<JArray*>(ptrArrItem)->length;
        dynamic_cast<JArray*>(objectref)->offset = dynamic_cast<JArray*>(ptrArrItem)->offset;
    }
    else if (typeid(*ptrArrItem) == typeid(JObject)) {
        objectref = new JObject;
        dynamic_cast<JObject*>(objectref)->jc = dynamic_cast<JObject*>(ptrArrItem)->jc;
        dynamic_cast<JObject*>(objectref)->offset = dynamic_cast<JObject*>(ptrArrItem)->offset;
    }
    currentFrame->stack.push(objectref);

    delete index;
    delete arrayref;
}

template <typename StoreType>
inline void CodeExecution::store2Local(u1 index) {
    auto* value = dynamic_cast<StoreType*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    currentFrame->locals[index] = value;
    if (IS_JLong(value) || IS_JDouble(value)) {
        currentFrame->locals[index + 1] = nullptr;
    }
}

template <>
inline void CodeExecution::store2Local<JRef>(u1 index) {
    JType* value = currentFrame->stack.top();
    currentFrame->stack.pop();
    currentFrame->locals[index] = value;
}

template <typename StoreType>
void CodeExecution::storeArrayItem() {
    //store array item retriving from stack to array heap
    StoreType* value = dynamic_cast<StoreType*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    JInt* index = (JInt*)currentFrame->stack.top();
    currentFrame->stack.pop();
    JArray* arrayref = (JArray*)currentFrame->stack.top();
    currentFrame->stack.pop();
    if (arrayref == nullptr) {
        throw std::runtime_error("null pointer");
    }
    if (index->val > arrayref->length || index->val < 0) {
        throw std::runtime_error("array index out of bounds");
    }
    yrt.jheap->putArrayItem(*arrayref, index->val, value);

    delete index;
    delete arrayref;
}

template <>
inline void CodeExecution::storeArrayItem<JRef>() {
    JType* value = currentFrame->stack.top();
    currentFrame->stack.pop();
    JInt* index = (JInt*)currentFrame->stack.top();
    currentFrame->stack.pop();
    JArray* arrayref = (JArray*)currentFrame->stack.top();
    currentFrame->stack.pop();
    if (arrayref == nullptr) {
        throw std::runtime_error("null pointer");
    }
    if (index->val > arrayref->length || index->val < 0) {
        throw std::runtime_error("array index out of bounds");
    }
    yrt.jheap->putArrayItem(*arrayref, index->val, value);

    delete index;
    delete arrayref;
}

template <typename ReturnType>
inline ReturnType* CodeExecution::flowReturn() const {
    auto* value = dynamic_cast<ReturnType*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    return value;
}

template <typename ReturnType>
inline ReturnType* CodeExecution::currentStackPop() {
    auto* value = dynamic_cast<ReturnType*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    return value;
}

template <typename Type1, typename Type2>
inline void CodeExecution::typeCast() const {
    auto * value = dynamic_cast<Type1*>(currentFrame->stack.top());
    currentFrame->stack.pop();
    auto * result = new Type2;
    result->val = value->val;
    currentFrame->stack.push(result);
    delete value;
}

#endif //YVM_CODEEXECUTION_H