#include "../classfile/AccessFlag.h"
#include "../classfile/ClassFile.h"
#include "../misc/Debug.h"
#include "../misc/Option.h"
#include "../runtime/JavaClass.h"
#include "../runtime/JavaHeap.hpp"
#include "CallSite.h"
#include "Interpreter.hpp"
#include "MethodResolve.h"
#include "SymbolicRef.h"

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>

using namespace std;

#define IS_COMPUTATIONAL_TYPE_1(value) \
    (typeid(*value) != typeid(JDouble) && typeid(*value) != typeid(JLong))
#define IS_COMPUTATIONAL_TYPE_2(value) \
    (typeid(*value) == typeid(JDouble) || typeid(*value) == typeid(JLong))

#pragma warning(disable : 4715)
#pragma warning(disable : 4244)

Interpreter::~Interpreter() { delete frames; }

JType *Interpreter::execNativeMethod(const string &className,
                                     const string &methodName,
                                     const string &methodDescriptor) {
    string nativeMethod(className);
    nativeMethod.append(".");
    nativeMethod.append(methodName);
    nativeMethod.append(".");
    nativeMethod.append(methodDescriptor);
    if (yrt.nativeMethods.find(nativeMethod) != yrt.nativeMethods.end()) {
        return ((*yrt.nativeMethods.find(nativeMethod)).second)(
            &yrt, frames->top()->localSlots, frames->top()->maxLocal);
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
    return nullptr;
}

JType *Interpreter::execByteCode(const JavaClass *jc, u1 *code, u4 codeLength,
                                 u2 exceptLen, ExceptionTable *exceptTab) {
    for (decltype(codeLength) op = 0; op < codeLength; op++) {
        // If callee propagates a unhandled exception, try to handle  it. When
        // we can not handle it, propagates it to upper and returns
        if (exception.hasUnhandledException()) {
            op--;
            auto *throwobj = frames->top()->pop<JObject>();
            if (throwobj == nullptr) {
                throw runtime_error("null pointer");
            }
            if (!hasInheritanceRelationship(
                    throwobj->jc,
                    yrt.ma->loadClassIfAbsent("java/lang/Throwable"))) {
                throw runtime_error("it's not a throwable object");
            }

            if (handleException(jc, exceptLen, exceptTab, throwobj, op)) {
                while (!frames->top()->emptyStack()) {
                    frames->top()->pop<JType>();
                }
                frames->top()->push(throwobj);
                exception.sweepException();
            } else {
                return throwobj;
            }
        }
#ifdef YVM_DEBUG_SHOW_BYTECODE
        for (int i = 0; i < frames.size(); i++) {
            cout << "-";
        }
        Inspector::printOpcode(code, op);
#endif
        // Interpreting through big switching
        switch (code[op]) {
            case op_nop: {
                // DO NOTHING :-)
            } break;
            case op_aconst_null: {
                frames->top()->push(nullptr);
            } break;
            case op_iconst_m1: {
                frames->top()->push(new JInt(-1));
            } break;
            case op_iconst_0: {
                frames->top()->push(new JInt(0));
            } break;
            case op_iconst_1: {
                frames->top()->push(new JInt(1));
            } break;
            case op_iconst_2: {
                frames->top()->push(new JInt(2));
            } break;
            case op_iconst_3: {
                frames->top()->push(new JInt(3));
            } break;
            case op_iconst_4: {
                frames->top()->push(new JInt(4));
            } break;
            case op_iconst_5: {
                frames->top()->push(new JInt(5));
            } break;
            case op_lconst_0: {
                frames->top()->push(new JLong(0));
            } break;
            case op_lconst_1: {
                frames->top()->push(new JLong(1));
            } break;
            case op_fconst_0: {
                frames->top()->push(new JFloat(0.0f));
            } break;
            case op_fconst_1: {
                frames->top()->push(new JFloat(1.0f));
            } break;
            case op_fconst_2: {
                frames->top()->push(new JFloat(2.0f));
            } break;
            case op_dconst_0: {
                frames->top()->push(new JDouble(0.0));
            } break;
            case op_dconst_1: {
                frames->top()->push(new JDouble(1.0));
            } break;
            case op_bipush: {
                const u1 byte = consumeU1(code, op);
                frames->top()->push(new JInt(byte));
            } break;
            case op_sipush: {
                const u2 byte = consumeU2(code, op);
                frames->top()->push(new JInt(byte));
            } break;
            case op_ldc: {
                const u1 index = consumeU1(code, op);
                loadConstantPoolItem2Stack(jc, static_cast<u2>(index));
            } break;
            case op_ldc_w: {
                const u2 index = consumeU2(code, op);
                loadConstantPoolItem2Stack(jc, index);
            } break;
            case op_ldc2_w: {
                const u2 index = consumeU2(code, op);
                if (typeid(*jc->raw.constPoolInfo[index]) ==
                    typeid(CONSTANT_Double)) {
                    auto val = dynamic_cast<CONSTANT_Double *>(
                                   jc->raw.constPoolInfo[index])
                                   ->val;
                    JDouble *dval = new JDouble;
                    dval->val = val;
                    frames->top()->push(dval);
                } else if (typeid(*jc->raw.constPoolInfo[index]) ==
                           typeid(CONSTANT_Long)) {
                    auto val = dynamic_cast<CONSTANT_Long *>(
                                   jc->raw.constPoolInfo[index])
                                   ->val;
                    JLong *lval = new JLong;
                    lval->val = val;
                    frames->top()->push(lval);
                } else {
                    throw runtime_error(
                        "invalid symbolic reference index on "
                        "constant pool");
                }
            } break;
            case op_iload: {
                const u1 index = consumeU1(code, op);
                frames->top()->load<JInt>(index);
            } break;
            case op_lload: {
                const u1 index = consumeU1(code, op);
                frames->top()->load<JLong>(index);
            } break;
            case op_fload: {
                const u1 index = consumeU1(code, op);
                frames->top()->load<JFloat>(index);
            } break;
            case op_dload: {
                const u1 index = consumeU1(code, op);
                frames->top()->load<JDouble>(index);
            } break;
            case op_aload: {
                const u1 index = consumeU1(code, op);
                frames->top()->load<JRef>(index);
            } break;
            case op_iload_0: {
                frames->top()->load<JInt>(0);
            } break;
            case op_iload_1: {
                frames->top()->load<JInt>(1);
            } break;
            case op_iload_2: {
                frames->top()->load<JInt>(2);
            } break;
            case op_iload_3: {
                frames->top()->load<JInt>(3);
            } break;
            case op_lload_0: {
                frames->top()->load<JLong>(0);
            } break;
            case op_lload_1: {
                frames->top()->load<JLong>(1);
            } break;
            case op_lload_2: {
                frames->top()->load<JLong>(2);
            } break;
            case op_lload_3: {
                frames->top()->load<JLong>(3);
            } break;
            case op_fload_0: {
                frames->top()->load<JFloat>(0);
            } break;
            case op_fload_1: {
                frames->top()->load<JFloat>(1);
            } break;
            case op_fload_2: {
                frames->top()->load<JFloat>(2);
            } break;
            case op_fload_3: {
                frames->top()->load<JFloat>(3);
            } break;
            case op_dload_0: {
                frames->top()->load<JDouble>(0);
            } break;
            case op_dload_1: {
                frames->top()->load<JDouble>(1);
            } break;
            case op_dload_2: {
                frames->top()->load<JDouble>(2);
            } break;
            case op_dload_3: {
                frames->top()->load<JDouble>(3);
            } break;
            case op_aload_0: {
                frames->top()->load<JRef>(0);
            } break;
            case op_aload_1: {
                frames->top()->load<JRef>(1);
            } break;
            case op_aload_2: {
                frames->top()->load<JRef>(2);
            } break;
            case op_aload_3: {
                frames->top()->load<JRef>(3);
            } break;
            case op_saload:
            case op_caload:
            case op_baload:
            case op_iaload: {
                auto *index = frames->top()->pop<JInt>();
                const auto *arrref = frames->top()->pop<JArray>();
                if (!arrref) {
                    throw runtime_error("nullpointerexception");
                }
                auto *elem = dynamic_cast<JInt *>(
                    yrt.jheap->getElement(*arrref, index->val));
                frames->top()->push(elem);
            } break;
            case op_laload: {
                auto *index = frames->top()->pop<JInt>();
                const auto *arrref = frames->top()->pop<JArray>();
                if (!arrref) {
                    throw runtime_error("nullpointerexception");
                }
                auto *elem = dynamic_cast<JLong *>(
                    yrt.jheap->getElement(*arrref, index->val));
                frames->top()->push(elem);
            } break;
            case op_faload: {
                auto *index = frames->top()->pop<JInt>();
                const auto *arrref = frames->top()->pop<JArray>();
                if (!arrref) {
                    throw runtime_error("nullpointerexception");
                }
                auto *elem = dynamic_cast<JFloat *>(
                    yrt.jheap->getElement(*arrref, index->val));
                frames->top()->push(elem);
            } break;
            case op_daload: {
                auto *index = frames->top()->pop<JInt>();
                const auto *arrref = frames->top()->pop<JArray>();
                if (!arrref) {
                    throw runtime_error("nullpointerexception");
                }
                auto *elem = dynamic_cast<JDouble *>(
                    yrt.jheap->getElement(*arrref, index->val));
                frames->top()->push(elem);
            } break;
            case op_aaload: {
                auto *index = frames->top()->pop<JInt>();
                const auto *arrref = frames->top()->pop<JArray>();
                if (!arrref) {
                    throw runtime_error("nullpointerexception");
                }
                auto *elem = dynamic_cast<JRef *>(
                    yrt.jheap->getElement(*arrref, index->val));
                frames->top()->push(elem);
            } break;
            case op_istore: {
                const u1 index = consumeU1(code, op);
                frames->top()->store<JInt>(index);
            } break;
            case op_lstore: {
                const u1 index = consumeU1(code, op);
                frames->top()->store<JLong>(index);
            } break;
            case op_fstore: {
                const u1 index = consumeU1(code, op);
                frames->top()->store<JFloat>(index);
            } break;
            case op_dstore: {
                const u1 index = consumeU1(code, op);
                frames->top()->store<JDouble>(index);
            } break;
            case op_astore: {
                const u1 index = consumeU1(code, op);
                frames->top()->store<JRef>(index);
            } break;
            case op_istore_0: {
                frames->top()->store<JInt>(0);
            } break;
            case op_istore_1: {
                frames->top()->store<JInt>(1);
            } break;
            case op_istore_2: {
                frames->top()->store<JInt>(2);
            } break;
            case op_istore_3: {
                frames->top()->store<JInt>(3);
            } break;
            case op_lstore_0: {
                frames->top()->store<JLong>(0);
            } break;
            case op_lstore_1: {
                frames->top()->store<JLong>(1);
            } break;
            case op_lstore_2: {
                frames->top()->store<JLong>(2);
            } break;
            case op_lstore_3: {
                frames->top()->store<JLong>(3);
            } break;
            case op_fstore_0: {
                frames->top()->store<JFloat>(0);
            } break;
            case op_fstore_1: {
                frames->top()->store<JFloat>(1);
            } break;
            case op_fstore_2: {
                frames->top()->store<JFloat>(2);
            } break;
            case op_fstore_3: {
                frames->top()->store<JFloat>(3);
            } break;
            case op_dstore_0: {
                frames->top()->store<JDouble>(0);
            } break;
            case op_dstore_1: {
                frames->top()->store<JDouble>(1);
            } break;
            case op_dstore_2: {
                frames->top()->store<JDouble>(2);
            } break;
            case op_dstore_3: {
                frames->top()->store<JDouble>(3);
            } break;
            case op_astore_0: {
                frames->top()->store<JRef>(0);
            } break;
            case op_astore_1: {
                frames->top()->store<JRef>(1);
            } break;
            case op_astore_2: {
                frames->top()->store<JRef>(2);
            } break;
            case op_astore_3: {
                frames->top()->store<JRef>(3);
            } break;
            case op_iastore: {
                auto *value = frames->top()->pop<JInt>();
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_lastore: {
                auto *value = frames->top()->pop<JLong>();
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_fastore: {
                auto *value = frames->top()->pop<JFloat>();
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_dastore: {
                auto *value = frames->top()->pop<JDouble>();
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_aastore: {
                auto *value = frames->top()->pop<JType>();
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_bastore: {
                auto *value = frames->top()->pop<JInt>();
                value->val = static_cast<int8_t>(value->val);
                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();

                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_sastore:
            case op_castore: {
                auto *value = frames->top()->pop<JInt>();
                value->val = static_cast<int16_t>(value->val);

                auto *index = frames->top()->pop<JInt>();
                auto *arrref = frames->top()->pop<JArray>();
                if (arrref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (index->val > arrref->length || index->val < 0) {
                    throw runtime_error("array index out of bounds");
                }
                yrt.jheap->putElement(*arrref, index->val, value);

            } break;
            case op_pop: {
                frames->top()->pop<JType>();
            } break;
            case op_pop2: {
                frames->top()->pop<JType>();
                frames->top()->pop<JType>();
            } break;
            case op_dup: {
                JType *value = frames->top()->pop<JType>();

                assert(typeid(*value) != typeid(JLong) &&
                       typeid(*value) != typeid(JDouble));
                frames->top()->push(value);
                frames->top()->push(cloneValue(value));
            } break;
            case op_dup_x1: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();

                assert(IS_COMPUTATIONAL_TYPE_1(value1));
                assert(IS_COMPUTATIONAL_TYPE_1(value2));

                frames->top()->push(cloneValue(value1));
                frames->top()->push(value2);
                frames->top()->push(value1);
            } break;
            case op_dup_x2: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();
                JType *value3 = frames->top()->pop<JType>();

                if (IS_COMPUTATIONAL_TYPE_1(value1) &&
                    IS_COMPUTATIONAL_TYPE_1(value2) &&
                    IS_COMPUTATIONAL_TYPE_1(value3)) {
                    // use structure 1
                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value3);
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else if (IS_COMPUTATIONAL_TYPE_1(value1) &&
                           IS_COMPUTATIONAL_TYPE_2(value2)) {
                    // use structure 2
                    frames->top()->push(value3);

                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_dup2: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();

                if (IS_COMPUTATIONAL_TYPE_1(value1) &&
                    IS_COMPUTATIONAL_TYPE_1(value2)) {
                    // use structure 1
                    frames->top()->push(cloneValue(value2));
                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else if (IS_COMPUTATIONAL_TYPE_2(value1)) {
                    // use structure 2
                    frames->top()->push(value2);

                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value1);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_dup2_x1: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();
                JType *value3 = frames->top()->pop<JType>();

                if (IS_COMPUTATIONAL_TYPE_1(value1) &&
                    IS_COMPUTATIONAL_TYPE_1(value2) &&
                    IS_COMPUTATIONAL_TYPE_1(value3)) {
                    // use structure 1
                    frames->top()->push(cloneValue(value2));
                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value3);
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else if (IS_COMPUTATIONAL_TYPE_2(value1) &&
                           IS_COMPUTATIONAL_TYPE_1(value2)) {
                    // use structure 2
                    frames->top()->push(value3);

                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_dup2_x2: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();
                JType *value3 = frames->top()->pop<JType>();
                JType *value4 = frames->top()->pop<JType>();
                if (IS_COMPUTATIONAL_TYPE_1(value1) &&
                    IS_COMPUTATIONAL_TYPE_1(value2) &&
                    IS_COMPUTATIONAL_TYPE_1(value3) &&
                    IS_COMPUTATIONAL_TYPE_1(value4)) {
                    // use structure 1
                    frames->top()->push(cloneValue(value2));
                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value4);
                    frames->top()->push(value3);
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else if (IS_COMPUTATIONAL_TYPE_2(value1) &&
                           IS_COMPUTATIONAL_TYPE_1(value2) &&
                           IS_COMPUTATIONAL_TYPE_1(value3)) {
                    // use structure 2
                    frames->top()->push(value4);

                    frames->top()->push(cloneValue(value1));
                    frames->top()->push(value4);
                    frames->top()->push(value2);
                    frames->top()->push(value1);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_swap: {
                JType *value1 = frames->top()->pop<JType>();
                JType *value2 = frames->top()->pop<JType>();

                assert(IS_COMPUTATIONAL_TYPE_1(value1));
                assert(IS_COMPUTATIONAL_TYPE_1(value2));
                if (typeid(*value1) == typeid(JInt) &&
                    typeid(*value2) == typeid(JInt)) {
                    swap(value1, value2);
                } else if (typeid(*value1) == typeid(JInt) &&
                           typeid(*value2) == typeid(JFloat)) {
                    const int32_t temp = dynamic_cast<JInt *>(value1)->val;
                    dynamic_cast<JInt *>(value1)->val = static_cast<int32_t>(
                        dynamic_cast<JFloat *>(value2)->val);
                    dynamic_cast<JFloat *>(value2)->val =
                        static_cast<float>(temp);
                } else if (typeid(*value1) == typeid(JFloat) &&
                           typeid(*value2) == typeid(JInt)) {
                    const float temp = dynamic_cast<JFloat *>(value1)->val;
                    dynamic_cast<JFloat *>(value1)->val =
                        static_cast<int32_t>(dynamic_cast<JInt *>(value2)->val);
                    dynamic_cast<JInt *>(value2)->val =
                        static_cast<int32_t>(temp);
                } else if (typeid(*value1) == typeid(JFloat) &&
                           typeid(*value2) == typeid(JFloat)) {
                    swap(value1, value2);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_iadd: {
                binaryArithmetic<JInt>(plus<>());
            } break;
            case op_ladd: {
                binaryArithmetic<JLong>(plus<>());
            } break;
            case op_fadd: {
                binaryArithmetic<JFloat>(plus<>());
            } break;
            case op_dadd: {
                binaryArithmetic<JDouble>(plus<>());
            } break;
            case op_isub: {
                binaryArithmetic<JInt>(minus<>());
            } break;
            case op_lsub: {
                binaryArithmetic<JLong>(minus<>());
            } break;
            case op_fsub: {
                binaryArithmetic<JFloat>(minus<>());
            } break;
            case op_dsub: {
                binaryArithmetic<JDouble>(minus<>());
            } break;
            case op_imul: {
                binaryArithmetic<JInt>(multiplies<>());
            } break;
            case op_lmul: {
                binaryArithmetic<JLong>(multiplies<>());
            } break;
            case op_fmul: {
                binaryArithmetic<JFloat>(multiplies<>());
            } break;
            case op_dmul: {
                binaryArithmetic<JDouble>(multiplies<>());
            } break;
            case op_idiv: {
                binaryArithmetic<JInt>(divides<>());
            } break;
            case op_ldiv: {
                binaryArithmetic<JLong>(divides<>());
            } break;
            case op_fdiv: {
                binaryArithmetic<JFloat>(divides<>());
            } break;
            case op_ddiv: {
                binaryArithmetic<JDouble>(divides<>());

            } break;
            case op_irem: {
                binaryArithmetic<JInt>(modulus<>());
            } break;
            case op_lrem: {
                binaryArithmetic<JLong>(modulus<>());
            } break;
            case op_frem: {
                binaryArithmetic<JFloat>(std::fmod<float, float>);
            } break;
            case op_drem: {
                binaryArithmetic<JFloat>(std::fmod<double, double>);
            } break;
            case op_ineg: {
                unaryArithmetic<JInt>(negate<>());
            } break;
            case op_lneg: {
                unaryArithmetic<JLong>(negate<>());
            } break;
            case op_fneg: {
                unaryArithmetic<JFloat>(negate<>());
            } break;
            case op_dneg: {
                unaryArithmetic<JDouble>(negate<>());
            } break;
            case op_ishl: {
                binaryArithmetic<JInt>([](int32_t a, int32_t b) -> int32_t {
                    return a * pow(2, b & 0x1f);
                });
            } break;
            case op_lshl: {
                binaryArithmetic<JLong>([](int64_t a, int64_t b) -> int64_t {
                    return a * pow(2, b & 0x3f);
                });
            } break;
            case op_ishr: {
                binaryArithmetic<JInt>([](int32_t a, int32_t b) -> int32_t {
                    return floor(a / pow(2, b & 0x1f));
                });
            } break;
            case op_lshr: {
                binaryArithmetic<JLong>([](int64_t a, int64_t b) -> int64_t {
                    return floor(a / pow(2, b & 0x3f));
                });
            } break;
            case op_iushr: {
                binaryArithmetic<JInt>([](int32_t a, int32_t b) -> int32_t {
                    if (a > 0) {
                        return a >> (b & 0x1f);
                    } else if (a < 0) {
                        return (a >> (b & 0x1f)) + (2 << ~(b & 0x1f));
                    } else {
                        throw runtime_error("0 is not handled");
                    }
                });
            } break;
            case op_lushr: {
                binaryArithmetic<JLong>([](int64_t a, int64_t b) -> int64_t {
                    if (a > 0) {
                        return a >> (b & 0x3f);
                    } else if (a < 0) {
                        return (a >> (b & 0x1f)) + (2L << ~(b & 0x3f));
                    } else {
                        throw runtime_error("0 is not handled");
                    }
                });
            } break;
            case op_iand: {
                binaryArithmetic<JInt>(bit_and<>());
            } break;
            case op_land: {
                binaryArithmetic<JLong>(bit_and<>());
            } break;
            case op_ior: {
                binaryArithmetic<JInt>(bit_or<>());
            } break;
            case op_lor: {
                binaryArithmetic<JLong>(bit_or<>());
            } break;
            case op_ixor: {
                binaryArithmetic<JInt>(bit_xor<>());
            } break;
            case op_lxor: {
                binaryArithmetic<JLong>(bit_xor<>());
            } break;
            case op_iinc: {
                const u1 index = code[++op];
                const int8_t count = code[++op];
                const int32_t extendedCount = count;
                if (IS_JINT(frames->top()->getLocalVariable(index))) {
                    dynamic_cast<JInt *>(frames->top()->getLocalVariable(index))
                        ->val += extendedCount;
                } else if (IS_JLong(frames->top()->getLocalVariable(index))) {
                    dynamic_cast<JLong *>(
                        frames->top()->getLocalVariable(index))
                        ->val += extendedCount;
                } else if (IS_JFloat(frames->top()->getLocalVariable(index))) {
                    dynamic_cast<JFloat *>(
                        frames->top()->getLocalVariable(index))
                        ->val += extendedCount;
                } else if (IS_JDouble(frames->top()->getLocalVariable(index))) {
                    dynamic_cast<JDouble *>(
                        frames->top()->getLocalVariable(index))
                        ->val += extendedCount;
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_i2l: {
                typeCast<JInt, JLong>();
            } break;
            case op_i2f: {
                typeCast<JInt, JFloat>();
            } break;
            case op_i2d: {
                typeCast<JInt, JDouble>();
            } break;
            case op_l2i: {
                typeCast<JLong, JInt>();
            } break;
            case op_l2f: {
                typeCast<JLong, JFloat>();
            } break;
            case op_l2d: {
                typeCast<JLong, JDouble>();
            } break;
            case op_f2i: {
                typeCast<JFloat, JInt>();
            } break;
            case op_f2l: {
                typeCast<JFloat, JLong>();
            } break;
            case op_f2d: {
                typeCast<JFloat, JDouble>();
            } break;
            case op_d2i: {
                typeCast<JDouble, JInt>();
            } break;
            case op_d2l: {
                typeCast<JDouble, JLong>();
            } break;
            case op_d2f: {
                typeCast<JDouble, JFloat>();
            } break;
            case op_i2c:
            case op_i2b: {
                auto *value = frames->top()->pop<JInt>();
                auto *result = new JInt;
                result->val = (int8_t)(value->val);
                frames->top()->push(result);

            } break;
            case op_i2s: {
                auto *value = frames->top()->pop<JInt>();
                auto *result = new JInt;
                result->val = (int16_t)(value->val);
                frames->top()->push(result);

            } break;
            case op_lcmp: {
                auto *value2 = frames->top()->pop<JLong>();
                auto *value1 = frames->top()->pop<JLong>();
                if (value1->val > value2->val) {
                    auto *result = new JInt(1);
                    frames->top()->push(result);
                } else if (value1->val == value2->val) {
                    auto *result = new JInt(0);
                    frames->top()->push(result);
                } else {
                    auto *result = new JInt(-1);
                    frames->top()->push(result);
                }

            } break;
            case op_fcmpg:
            case op_fcmpl: {
                auto *value2 = frames->top()->pop<JFloat>();
                auto *value1 = frames->top()->pop<JFloat>();
                if (value1->val > value2->val) {
                    auto *result = new JInt(1);
                    frames->top()->push(result);
                } else if (abs(value1->val - value2->val) < 0.000001) {
                    auto *result = new JInt(0);
                    frames->top()->push(result);
                } else {
                    auto *result = new JInt(-1);
                    frames->top()->push(result);
                }

            } break;
            case op_dcmpl:
            case op_dcmpg: {
                auto *value2 = frames->top()->pop<JDouble>();
                auto *value1 = frames->top()->pop<JDouble>();
                if (value1->val > value2->val) {
                    auto *result = new JInt(1);
                    frames->top()->push(result);
                } else if (abs(value1->val - value2->val) < 0.000000000001) {
                    auto *result = new JInt(0);
                    frames->top()->push(result);
                } else {
                    auto *result = new JInt(-1);
                    frames->top()->push(result);
                }

            } break;
            case op_ifeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val == 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_ifne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val != 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_iflt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val < 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_ifge: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val >= 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_ifgt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val > 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_ifle: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value = frames->top()->pop<JInt>();
                if (value->val <= 0) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmpeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val == value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmpne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val != value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmplt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val < value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmpge: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val >= value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmpgt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val > value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_icmple: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JInt>();
                auto *value1 = frames->top()->pop<JInt>();
                if (value1->val <= value2->val) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_acmpeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JObject>();
                auto *value1 = frames->top()->pop<JObject>();
                if (value1->offset == value2->offset &&
                    value1->jc == value2->jc) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_if_acmpne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                auto *value2 = frames->top()->pop<JObject>();
                auto *value1 = frames->top()->pop<JObject>();
                if (value1->offset != value2->offset ||
                    value1->jc != value2->jc) {
                    op = currentOffset + branchindex;
                }

            } break;
            case op_goto: {
                u4 currentOffset = op - 1;
                int16_t branchindex = consumeU2(code, op);
                op = currentOffset + branchindex;
            } break;
            case op_jsr: {
                throw runtime_error("unsupported opcode [jsr]");
            } break;
            case op_ret: {
                throw runtime_error("unsupported opcode [ret]");
            } break;
            case op_tableswitch: {
                u4 currentOffset = op - 1;
                op++;
                op++;
                op++;  // 3 bytes padding
                int32_t defaultIndex = consumeU4(code, op);
                int32_t low = consumeU4(code, op);
                int32_t high = consumeU4(code, op);
                vector<int32_t> jumpOffset;
                FOR_EACH(i, high - low + 1) {
                    jumpOffset.push_back(consumeU4(code, op));
                }

                auto *index = frames->top()->pop<JInt>();
                if (index->val < low || index->val > high) {
                    op = currentOffset + defaultIndex;
                } else {
                    op = currentOffset + jumpOffset[index->val - low];
                }

            } break;
            case op_lookupswitch: {
                u4 currentOffset = op - 1;
                op++;
                op++;
                op++;  // 3 bytes padding
                int32_t defaultIndex = consumeU4(code, op);
                int32_t npair = consumeU4(code, op);
                map<int32_t, int32_t> matchOffset;
                FOR_EACH(i, npair) {
                    matchOffset.insert(
                        make_pair(consumeU4(code, op), consumeU4(code, op)));
                }
                auto *key = frames->top()->pop<JInt>();
                auto res = matchOffset.find(key->val);
                if (res != matchOffset.end()) {
                    op = currentOffset + (*res).second;
                } else {
                    op = currentOffset + defaultIndex;
                }
            } break;
            case op_ireturn: {
                return cloneValue(frames->top()->pop<JInt>());
            } break;
            case op_lreturn: {
                return cloneValue(frames->top()->pop<JLong>());
            } break;
            case op_freturn: {
                return cloneValue(frames->top()->pop<JFloat>());
            } break;
            case op_dreturn: {
                return cloneValue(frames->top()->pop<JDouble>());
            } break;
            case op_areturn: {
                return cloneValue(frames->top()->pop<JType>());
            } break;
            case op_return: {
                return nullptr;
            } break;
            case op_getstatic: {
                const u2 index = consumeU2(code, op);
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                yrt.ma->linkClassIfAbsent(symbolicRef.jc->getClassName());
                yrt.ma->initClassIfAbsent(*this,
                                          symbolicRef.jc->getClassName());
                JType *field = symbolicRef.jc->getStaticVar(
                    symbolicRef.name, symbolicRef.descriptor);
                frames->top()->push(field);
            } break;
            case op_putstatic: {
                u2 index = consumeU2(code, op);
                JType *value = frames->top()->pop<JType>();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);

                yrt.ma->linkClassIfAbsent(symbolicRef.jc->getClassName());
                yrt.ma->initClassIfAbsent(*this,
                                          symbolicRef.jc->getClassName());
                symbolicRef.jc->setStaticVar(symbolicRef.name,
                                             symbolicRef.descriptor, value);
            } break;
            case op_getfield: {
                u2 index = consumeU2(code, op);
                JObject *objectref = frames->top()->pop<JObject>();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                JType *field = cloneValue(yrt.jheap->getFieldByName(
                    symbolicRef.jc, symbolicRef.name, symbolicRef.descriptor,
                    objectref));
                frames->top()->push(field);

            } break;
            case op_putfield: {
                const u2 index = consumeU2(code, op);
                JType *value = frames->top()->pop<JType>();
                JObject *objectref = frames->top()->pop<JObject>();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                yrt.jheap->putFieldByName(symbolicRef.jc, symbolicRef.name,
                                          symbolicRef.descriptor, objectref,
                                          value);

            } break;
            case op_invokevirtual: {
                const u2 index = consumeU2(code, op);
                assert(typeid(*jc->raw.constPoolInfo[index]) ==
                       typeid(CONSTANT_Methodref));

                auto symbolicRef = parseMethodSymbolicReference(jc, index);

                if (symbolicRef.name == "<init>") {
                    runtime_error(
                        "invoking method should not be instance "
                        "initialization method\n");
                }
                if (!IS_SIGNATURE_POLYMORPHIC_METHOD(
                        symbolicRef.jc->getClassName(), symbolicRef.name)) {
                    invokeVirtual(symbolicRef.name, symbolicRef.descriptor);
                } else {
                    // TODO:TO BE IMPLEMENTED
                }

            } break;
            case op_invokespecial: {
                const u2 index = consumeU2(code, op);
                SymbolicRef symbolicRef;

                if (typeid(*jc->raw.constPoolInfo[index]) ==
                    typeid(CONSTANT_InterfaceMethodref)) {
                    symbolicRef =
                        parseInterfaceMethodSymbolicReference(jc, index);
                } else if (typeid(*jc->raw.constPoolInfo[index]) ==
                           typeid(CONSTANT_Methodref)) {
                    symbolicRef = parseMethodSymbolicReference(jc, index);
                } else {
                    SHOULD_NOT_REACH_HERE
                }

                // If all of the following are true, let C be the direct
                // superclass of the current class :
                JavaClass *symbolicRefClass = symbolicRef.jc;
                if ("<init>" != symbolicRef.name) {
                    if (!IS_CLASS_INTERFACE(
                            symbolicRefClass->raw.accessFlags)) {
                        if (symbolicRefClass->getClassName() ==
                            jc->getSuperClassName()) {
                            if (IS_CLASS_SUPER(jc->raw.accessFlags)) {
                                invokeSpecial(yrt.ma->findJavaClass(
                                                  jc->getSuperClassName()),
                                              symbolicRef.name,
                                              symbolicRef.descriptor);
                                break;
                            }
                        }
                    }
                }
                // Otherwise let C be the symbolic reference class
                invokeSpecial(symbolicRef.jc, symbolicRef.name,
                              symbolicRef.descriptor);
            } break;
            case op_invokestatic: {
                // Invoke a class (static) method
                const u2 index = consumeU2(code, op);

                if (typeid(*jc->raw.constPoolInfo[index]) ==
                    typeid(CONSTANT_InterfaceMethodref)) {
                    auto symbolicRef =
                        parseInterfaceMethodSymbolicReference(jc, index);
                    invokeStatic(symbolicRef.jc, symbolicRef.name,
                                 symbolicRef.descriptor);
                } else if (typeid(*jc->raw.constPoolInfo[index]) ==
                           typeid(CONSTANT_Methodref)) {
                    auto symbolicRef = parseMethodSymbolicReference(jc, index);
                    invokeStatic(symbolicRef.jc, symbolicRef.name,
                                 symbolicRef.descriptor);
                } else {
                    SHOULD_NOT_REACH_HERE
                }
            } break;
            case op_invokeinterface: {
                const u2 index = consumeU2(code, op);
                ++op;  // read count and discard
                ++op;  // opcode padding 0;

                if (typeid(*jc->raw.constPoolInfo[index]) ==
                    typeid(CONSTANT_InterfaceMethodref)) {
                    auto symbolicRef =
                        parseInterfaceMethodSymbolicReference(jc, index);
                    invokeInterface(symbolicRef.jc, symbolicRef.name,
                                    symbolicRef.descriptor);
                }
            } break;
            case op_invokedynamic: {
                throw runtime_error("unsupported opcode [invokedynamic]");
            } break;
            case op_new: {
                const u2 index = consumeU2(code, op);
                JObject *objectref = execNew(jc, index);
                frames->top()->push(objectref);
            } break;
            case op_newarray: {
                const u1 atype = code[++op];
                JInt *count = frames->top()->pop<JInt>();

                if (count->val < 0) {
                    throw runtime_error("negative array size");
                }
                JArray *arrayref = yrt.jheap->createPODArray(atype, count->val);

                frames->top()->push(arrayref);

            } break;
            case op_anewarray: {
                const u2 index = consumeU2(code, op);
                auto symbolicRef = parseClassSymbolicReference(jc, index);
                JInt *count = frames->top()->pop<JInt>();

                if (count->val < 0) {
                    throw runtime_error("negative array size");
                }
                JArray *arrayref =
                    yrt.jheap->createObjectArray(*symbolicRef.jc, count->val);

                frames->top()->push(arrayref);
            } break;
            case op_arraylength: {
                JArray *arrayref = frames->top()->pop<JArray>();

                if (arrayref == nullptr) {
                    throw runtime_error("null pointer\n");
                }
                JInt *length = new JInt;
                length->val = arrayref->length;
                frames->top()->push(length);

            } break;
            case op_athrow: {
                auto *throwobj = frames->top()->pop<JObject>();
                if (throwobj == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (!hasInheritanceRelationship(
                        throwobj->jc,
                        yrt.ma->loadClassIfAbsent("java/lang/Throwable"))) {
                    throw runtime_error("it's not a throwable object");
                }

                if (handleException(jc, exceptLen, exceptTab, throwobj, op)) {
                    while (!frames->top()->emptyStack()) {
                        frames->top()->pop<JType>();
                    }
                    frames->top()->push(throwobj);
                } else /* Exception can not handled within method handlers */ {
                    exception.markException();
                    exception.setThrowExceptionInfo(throwobj);
                    return throwobj;
                }
            } break;
            case op_checkcast: {
                throw runtime_error("unsupported opcode [checkcast]");
            } break;
            case op_instanceof: {
                const u2 index = consumeU2(code, op);
                auto *objectref = frames->top()->pop<JObject>();
                if (objectref == nullptr) {
                    frames->top()->push(new JInt(0));
                }
                if (checkInstanceof(jc, index, objectref)) {
                    frames->top()->push(new JInt(1));
                } else {
                    frames->top()->push(new JInt(0));
                }
            } break;
            case op_monitorenter: {
                JType *ref = frames->top()->pop<JType>();

                if (ref == nullptr) {
                    throw runtime_error("null pointer");
                }

                if (!yrt.jheap->hasMonitor(ref)) {
                    dynamic_cast<JObject *>(ref)->offset =
                        yrt.jheap->createMonitor();
                }
                yrt.jheap->findMonitor(ref)->enter(this_thread::get_id());
            } break;
            case op_monitorexit: {
                JType *ref = frames->top()->pop<JType>();

                if (ref == nullptr) {
                    throw runtime_error("null pointer");
                }
                if (!yrt.jheap->hasMonitor(ref)) {
                    dynamic_cast<JObject *>(ref)->offset =
                        yrt.jheap->createMonitor();
                }
                yrt.jheap->findMonitor(ref)->exit();

            } break;
            case op_wide: {
                throw runtime_error("unsupported opcode [wide]");
            } break;
            case op_multianewarray: {
                throw runtime_error("unsupported opcode [multianewarray]");
            } break;
            case op_ifnull: {
                u4 currentOffset = op - 1;
                int16_t branchIndex = consumeU2(code, op);
                JObject *value = frames->top()->pop<JObject>();
                if (value == nullptr) {
                    op = currentOffset + branchIndex;
                }
            } break;
            case op_ifnonnull: {
                u4 currentOffset = op - 1;
                int16_t branchIndex = consumeU2(code, op);
                JObject *value = frames->top()->pop<JObject>();
                if (value != nullptr) {
                    op = currentOffset + branchIndex;
                }
            } break;
            case op_goto_w: {
                u4 currentOffset = op - 1;
                int32_t branchIndex = consumeU4(code, op);
                op = currentOffset + branchIndex;
            } break;
            case op_jsr_w: {
                throw runtime_error("unsupported opcode [jsr_w]");
            } break;
            case op_breakpoint:
            case op_impdep1:
            case op_impdep2: {
                // Reserved opcodde
                cerr << "Are you a dot.class hacker? Or you were entered a "
                        "strange region.";
                exit(EXIT_FAILURE);
            } break;
            default:
                cerr << "The YVM can not recognize this opcode. Bytecode file "
                        "was be corrupted.";
                exit(EXIT_FAILURE);
        }
    }
    return nullptr;
}
//--------------------------------------------------------------------------------
//  This function does "ldc" opcode jc type of JavaClass, which indicate where
//  to resolve
//--------------------------------------------------------------------------------
void Interpreter::loadConstantPoolItem2Stack(const JavaClass *jc, u2 index) {
    if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Integer)) {
        auto val =
            dynamic_cast<CONSTANT_Integer *>(jc->raw.constPoolInfo[index])->val;
        JInt *ival = new JInt;
        ival->val = val;
        frames->top()->push(ival);
    } else if (typeid(*jc->raw.constPoolInfo[index]) ==
               typeid(CONSTANT_Float)) {
        auto val =
            dynamic_cast<CONSTANT_Float *>(jc->raw.constPoolInfo[index])->val;
        JFloat *fval = new JFloat;
        fval->val = val;
        frames->top()->push(fval);
    } else if (typeid(*jc->raw.constPoolInfo[index]) ==
               typeid(CONSTANT_String)) {
        auto val = jc->getString(
            dynamic_cast<CONSTANT_String *>(jc->raw.constPoolInfo[index])
                ->stringIndex);
        JObject *str = yrt.jheap->createObject(
            *yrt.ma->loadClassIfAbsent("java/lang/String"));
        JArray *value = yrt.jheap->createCharArray(val, val.length());
        // Put string  into str's field; according the source file of
        // java.lang.Object, we know that its first field was used to store
        // chars
        yrt.jheap->putFieldByOffset(*str, 0, value);
        frames->top()->push(str);
    } else if (typeid(*jc->raw.constPoolInfo[index]) ==
               typeid(CONSTANT_Class)) {
        throw runtime_error("nonsupport region");
    } else if (typeid(*jc->raw.constPoolInfo[index]) ==
               typeid(CONSTANT_MethodType)) {
        throw runtime_error("nonsupport region");
    } else if (typeid(*jc->raw.constPoolInfo[index]) ==
               typeid(CONSTANT_MethodHandle)) {
        throw runtime_error("nonsupport region");
    } else {
        throw runtime_error(
            "invalid symbolic reference index on constant "
            "pool");
    }
}

bool Interpreter::handleException(const JavaClass *jc, u2 exceptLen,
                                  ExceptionTable *exceptTab,
                                  const JObject *objectref, u4 &op) {
    FOR_EACH(i, exceptLen) {
        const string &catchTypeName =
            jc->getString(dynamic_cast<CONSTANT_Class *>(
                              jc->raw.constPoolInfo[exceptTab[i].catchType])
                              ->nameIndex);

        if (hasInheritanceRelationship(
                yrt.ma->findJavaClass(objectref->jc->getClassName()),
                yrt.ma->findJavaClass(catchTypeName)) &&
            exceptTab[i].startPC <= op && op < exceptTab[i].endPC) {
            // start<=op<end
            // If we found a proper exception handler, set current pc as
            // handlerPC of this exception table item;
            op = exceptTab[i].handlerPC - 1;
            return true;
        }
        if (exceptTab[i].catchType == 0) {
            op = exceptTab[i].handlerPC - 1;
            return true;
        }
    }

    return false;
}

JObject *Interpreter::execNew(const JavaClass *jc, u2 index) {
    yrt.ma->linkClassIfAbsent(const_cast<JavaClass *>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this,
                              const_cast<JavaClass *>(jc)->getClassName());

    if (typeid(*jc->raw.constPoolInfo[index]) != typeid(CONSTANT_Class)) {
        throw runtime_error(
            "operand index of new is not a class or "
            "interface\n");
    }
    string className = jc->getString(
        dynamic_cast<CONSTANT_Class *>(jc->raw.constPoolInfo[index])
            ->nameIndex);
    JavaClass *newClass = yrt.ma->loadClassIfAbsent(className);
    return yrt.jheap->createObject(*newClass);
}

bool Interpreter::checkInstanceof(const JavaClass *jc, u2 index,
                                  JType *objectref) {
    string TclassName =
        (char *)dynamic_cast<CONSTANT_Utf8 *>(
            jc->raw.constPoolInfo[dynamic_cast<CONSTANT_Class *>(
                                      jc->raw.constPoolInfo[index])
                                      ->nameIndex])
            ->bytes;
    constexpr short TYPE_ARRAY = 1;
    constexpr short TYPE_CLASS = 2;
    constexpr short TYPE_INTERFACE = 3;

    short tType = 0;
    if (TclassName.find('[') != string::npos) {
        tType = TYPE_ARRAY;
    } else {
        if (IS_CLASS_INTERFACE(
                yrt.ma->findJavaClass(TclassName)->raw.accessFlags)) {
            tType = TYPE_INTERFACE;
        } else {
            tType = TYPE_CLASS;
        }
    }

    if (typeid(objectref) == typeid(JObject)) {
        if (!IS_CLASS_INTERFACE(
                dynamic_cast<JObject *>(objectref)->jc->raw.accessFlags)) {
            // If it's an ordinary class
            if (tType == TYPE_CLASS) {
                if (yrt.ma->findJavaClass(dynamic_cast<JObject *>(objectref)
                                              ->jc->getClassName())
                            ->getClassName() == TclassName ||
                    yrt.ma->findJavaClass(dynamic_cast<JObject *>(objectref)
                                              ->jc->getSuperClassName())
                            ->getClassName() == TclassName) {
                    return true;
                }
            } else if (tType == TYPE_INTERFACE) {
                auto &&interfaceIdxs = dynamic_cast<JObject *>(objectref)
                                           ->jc->getInterfacesIndex();
                FOR_EACH(i, interfaceIdxs.size()) {
                    string interfaceName =
                        dynamic_cast<JObject *>(objectref)->jc->getString(
                            dynamic_cast<CONSTANT_Class *>(
                                dynamic_cast<JObject *>(objectref)
                                    ->jc->raw.constPoolInfo[interfaceIdxs[i]])
                                ->nameIndex);
                    if (interfaceName == TclassName) {
                        return true;
                    }
                }
            } else {
                SHOULD_NOT_REACH_HERE
            }
        } else {
            // Otherwise, it's an interface class
            if (tType == TYPE_CLASS) {
                if (TclassName == "java/lang/Object") {
                    return true;
                }
            } else if (tType == TYPE_INTERFACE) {
                if (TclassName == dynamic_cast<JObject *>(objectref)
                                      ->jc->getClassName() ||
                    TclassName == dynamic_cast<JObject *>(objectref)
                                      ->jc->getSuperClassName()) {
                    return true;
                }
            } else {
                SHOULD_NOT_REACH_HERE
            }
        }
    } else if (typeid(objectref) == typeid(JArray)) {
        if (tType == TYPE_CLASS) {
            if (TclassName == "java/lang/Object") {
                return true;
            }
        } else if (tType == TYPE_INTERFACE) {
            auto *firstComponent = dynamic_cast<JObject *>(
                yrt.jheap->getElement(*dynamic_cast<JArray *>(objectref), 0));
            auto &&interfaceIdxs = firstComponent->jc->getInterfacesIndex();
            FOR_EACH(i, interfaceIdxs.size()) {
                if (firstComponent->jc->getString(
                        dynamic_cast<CONSTANT_Class *>(
                            firstComponent->jc->raw
                                .constPoolInfo[interfaceIdxs[i]])
                            ->nameIndex) == TclassName) {
                    return true;
                }
            }
        } else if (tType == TYPE_ARRAY) {
            throw runtime_error("to be continue\n");
        } else {
            SHOULD_NOT_REACH_HERE
        }
    } else {
        SHOULD_NOT_REACH_HERE
    }
}

void Interpreter::pushMethodArguments(vector<int> &parameter,
                                      bool isObjectMethod) {
    if (parameter.size() > 0) {
        for (int localIndex = parameter.size() - (isObjectMethod ? 0 : 1),
                 paramIndex = parameter.size() - 1;
             paramIndex >= 0; localIndex--, paramIndex--) {
            if (parameter[paramIndex] == T_INT ||
                parameter[paramIndex] == T_BOOLEAN ||
                parameter[paramIndex] == T_CHAR ||
                parameter[paramIndex] == T_BYTE ||
                parameter[paramIndex] == T_SHORT) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JInt>());
            } else if (parameter[paramIndex] == T_FLOAT) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JFloat>());
            } else if (parameter[paramIndex] == T_DOUBLE) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JDouble>());
            } else if (parameter[paramIndex] == T_LONG) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JLong>());
            } else if (parameter[paramIndex] == T_EXTRA_ARRAY) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JArray>());
            } else if (parameter[paramIndex] == T_EXTRA_OBJECT) {
                frames->top()->setLocalVariable(
                    localIndex, frames->nextFrame()->pop<JObject>());
            } else {
                SHOULD_NOT_REACH_HERE;
            }
        }
    }
    if (isObjectMethod) {
        frames->top()->setLocalVariable(0, frames->nextFrame()->pop<JObject>());
    }
}
//--------------------------------------------------------------------------------
// Invoke by given name, this method was be used internally
//--------------------------------------------------------------------------------
void Interpreter::invokeByName(JavaClass *jc, const string &name,
                               const string &descriptor) {
    const int returnType = get<0>(peelMethodParameterAndType(descriptor));

    MethodInfo *m = jc->findMethod(name, descriptor);
    CallSite csite = CallSite::makeCallSite(jc, m);
    if (!csite.isCallable()) {
        return;
    }

    frames->pushFrame(csite.maxLocal, csite.maxStack);

    JType *returnValue{};
    if (IS_METHOD_NATIVE(m->accessFlags)) {
        returnValue =
            cloneValue(execNativeMethod(jc->getClassName(), name, descriptor));
    } else {
        returnValue =
            cloneValue(execByteCode(jc, csite.code, csite.codeLength,
                                    csite.exceptionLen, csite.exception));
    }
    frames->popFrame();

    // Since invokeByName() was merely used to call <clinit> and main method
    // of running program, therefore, if an exception reached here, we don't
    // need to push its value into upper frame  again (In fact there is no more
    // frame), we just print stack trace inforamtion to notice user and
    // return directly
    if (returnType != T_EXTRA_VOID) {
        frames->top()->push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        exception.extendExceptionStackTrace(name);
        exception.printStackTrace();
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
}
//--------------------------------------------------------------------------------
// Invoke interface method
//--------------------------------------------------------------------------------
void Interpreter::invokeInterface(const JavaClass *jc, const string &name,
                                  const string &descriptor) {
    auto parameterAndReturnType = peelMethodParameterAndType(descriptor);
    const int returnType = get<0>(parameterAndReturnType);
    auto parameter = get<1>(parameterAndReturnType);

    auto csite = findInstanceMethod(jc, name, descriptor);
    if (!csite.isCallable()) {
        csite = findInstanceMethodOnSupers(jc, name, descriptor);
        if (!csite.isCallable()) {
            csite = findMaximallySpecifiedMethod(jc, name, descriptor);
            if (!csite.isCallable()) {
                throw runtime_error("can not find method " + name + " " +
                                    descriptor);
            }
        }
    }

    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        csite.maxLocal = csite.maxStack = parameter.size() + 1;
    }
    frames->pushFrame(csite.maxLocal, csite.maxStack);
    pushMethodArguments(parameter, true);

    JType *returnValue{};
    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        returnValue = cloneValue(
            execNativeMethod(csite.jc->getClassName(), name, descriptor));
    } else {
        returnValue =
            cloneValue(execByteCode(csite.jc, csite.code, csite.codeLength,
                                    csite.exceptionLen, csite.exception));
    }
    frames->popFrame();

    if (returnType != T_EXTRA_VOID) {
        frames->top()->push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        frames->top()->grow(1);
        frames->top()->push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(name);
        }
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
}

//--------------------------------------------------------------------------------
// Invoke instance method; dispatch based on class
//--------------------------------------------------------------------------------
void Interpreter::invokeVirtual(const string &name, const string &descriptor) {
    auto parameterAndReturnType = peelMethodParameterAndType(descriptor);
    const int returnType = get<0>(parameterAndReturnType);
    auto parameter = get<1>(parameterAndReturnType);

    auto *thisRef =
        (JObject *)frames->top()
            ->stackSlots[frames->top()->stackTop - parameter.size() - 1];

    auto csite = findInstanceMethod(thisRef->jc, name, descriptor);
    if (!csite.isCallable()) {
        csite = findInstanceMethodOnSupers(thisRef->jc, name, descriptor);
        if (!csite.isCallable()) {
            csite = findMaximallySpecifiedMethod(thisRef->jc, name, descriptor);
            if (!csite.isCallable()) {
                throw runtime_error("can not find method " + name + " " +
                                    descriptor);
            }
        }
    }

    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        csite.maxLocal = csite.maxStack = parameter.size() + 1;
    }

    frames->pushFrame(csite.maxLocal, csite.maxStack);
    pushMethodArguments(parameter, true);
    JType *returnValue{};
    if (csite.isCallable()) {
        if (IS_METHOD_NATIVE(csite.accessFlags)) {
            returnValue = cloneValue(
                execNativeMethod(csite.jc->getClassName(), name, descriptor));
        } else {
            returnValue =
                cloneValue(execByteCode(csite.jc, csite.code, csite.codeLength,
                                        csite.exceptionLen, csite.exception));
        }
    } else {
        throw runtime_error("can not find method to call");
    }
    frames->popFrame();

    if (returnType != T_EXTRA_VOID) {
        frames->top()->push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        frames->top()->grow(1);
        frames->top()->push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(name);
        }
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
}
//--------------------------------------------------------------------------------
//  Invoke instance method; special handling for superclass, private,
//  and instance initialization method invocations
//--------------------------------------------------------------------------------
void Interpreter::invokeSpecial(const JavaClass *jc, const string &name,
                                const string &descriptor) {
    auto parameterAndReturnType = peelMethodParameterAndType(descriptor);
    const int returnType = get<0>(parameterAndReturnType);
    auto parameter = get<1>(parameterAndReturnType);

    auto csite = findInstanceMethod(jc, name, descriptor);
    if (!csite.isCallable()) {
        csite = findInstanceMethodOnSupers(jc, name, descriptor);
        if (!csite.isCallable()) {
            csite = findJavaLangObjectMethod(jc, name, descriptor);
            if (!csite.isCallable()) {
                csite = findMaximallySpecifiedMethod(jc, name, descriptor);
                if (!csite.isCallable()) {
                    throw runtime_error("can not find method " + name + " " +
                                        descriptor);
                }
            }
        }
    }
    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        csite.maxLocal = csite.maxStack = parameter.size() + 1;
    }
    frames->pushFrame(csite.maxLocal, csite.maxStack);
    pushMethodArguments(parameter, true);
    JType *returnValue{};

    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        returnValue = cloneValue(
            execNativeMethod(csite.jc->getClassName(), name, descriptor));
    } else {
        returnValue =
            cloneValue(execByteCode(csite.jc, csite.code, csite.codeLength,
                                    csite.exceptionLen, csite.exception));
    }
    frames->popFrame();
    if (returnType != T_EXTRA_VOID) {
        frames->top()->push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        frames->top()->grow(1);
        frames->top()->push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(name);
        }
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
}

void Interpreter::invokeStatic(const JavaClass *jc, const string &name,
                               const string &descriptor) {
    // Get instance method name and descriptor from CONSTANT_Methodref
    // locating by index and get interface method parameter and return value
    // descriptor
    yrt.ma->linkClassIfAbsent(const_cast<JavaClass *>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this,
                              const_cast<JavaClass *>(jc)->getClassName());

    auto parameterAndReturnType = peelMethodParameterAndType(descriptor);
    const int returnType = get<0>(parameterAndReturnType);
    auto parameter = get<1>(parameterAndReturnType);

    auto csite = CallSite::makeCallSite(jc, jc->findMethod(name, descriptor));
    if (!csite.isCallable()) {
        throw runtime_error("can not find method " + name + " " + descriptor);
    }
    assert(IS_METHOD_STATIC(csite.accessFlags) == true);
    assert(IS_METHOD_ABSTRACT(csite.accessFlags) == false);
    assert("<init>" != name);

    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        csite.maxLocal = csite.maxStack = parameter.size();
    }
    frames->pushFrame(csite.maxLocal, csite.maxStack);
    pushMethodArguments(parameter, false);
    JType *returnValue{};
    if (IS_METHOD_NATIVE(csite.accessFlags)) {
        returnValue = cloneValue(
            execNativeMethod(csite.jc->getClassName(), name, descriptor));
    } else {
        returnValue =
            cloneValue(execByteCode(csite.jc, csite.code, csite.codeLength,
                                    csite.exceptionLen, csite.exception));
    }
    frames->popFrame();

    if (returnType != T_EXTRA_VOID) {
        frames->top()->push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        frames->top()->grow(1);
        frames->top()->push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(name);
        }
    }

    GC_SAFE_POINT
    if (yrt.gc->shallGC()) {
        yrt.gc->stopTheWorld();
        yrt.gc->gc(frames, GCPolicy::GC_MARK_AND_SWEEP);
    }
}
