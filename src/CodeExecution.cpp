#include "CodeExecution.hpp"
#include "Debug.h"
#include "AccessFlag.h"
#include "ClassFile.h"
#include <iostream>
#include "Opcode.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "Option.h"
#include <cassert>
#include "Descriptor.h"
#include <cmath>

JType * CodeExecution::execCode(const JavaClass * jc, CodeExtension ext) {
    for (decltype(ext.codeLength) op = 0; op < ext.codeLength; op++) {
        yrt.eip = op;
#ifdef YVM_DEBUG_SHOW_BYTECODE
        for (int i = 0; i<yrt.frames.size(); i++) {
            std::cout << "-";
        }
        Inspector::printOpcode(ext.code, op);
#endif
        switch (ext.code[op]) {
            case op_nop: {
                // DO NOTHING :-)
            }break;
            case op_aconst_null: {
                JObject * obj = nullptr;
                currentFrame->stack.push(obj);
            }break;
            case op_iconst_m1: {
                JInt * ival = new JInt;
                ival->val = -1;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_0: {
                JInt * ival = new JInt;
                ival->val = 0;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_1: {
                JInt * ival = new JInt;
                ival->val = 1;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_2: {
                JInt * ival = new JInt;
                ival->val = 2;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_3: {
                JInt * ival = new JInt;
                ival->val = 3;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_4: {
                JInt * ival = new JInt;
                ival->val = 4;
                currentFrame->stack.push(ival);
            }break;
            case op_iconst_5: {
                JInt * ival = new JInt;
                ival->val = 5;
                currentFrame->stack.push(ival);
            }break;
            case op_lconst_0: {
                JLong * lval = new JLong;
                lval->val = 0;
                currentFrame->stack.push(lval);
            }break;
            case op_lconst_1: {
                JLong * lval = new JLong;
                lval->val = 1;
                currentFrame->stack.push(lval);
            }break;
            case op_fconst_0: {
                JFloat * fval = new JFloat;
                fval->val = 0.0f;
                currentFrame->stack.push(fval);
            }break;
            case op_fconst_1: {
                JFloat * fval = new JFloat;
                fval->val = 1.0f;
                currentFrame->stack.push(fval);
            }break;
            case op_fconst_2: {
                JFloat * fval = new JFloat;
                fval->val = 2.0f;
                currentFrame->stack.push(fval);
            }break;
            case op_dconst_0: {
                JDouble * dval = new JDouble;
                dval->val = 0.0;
                currentFrame->stack.push(dval);
            }break;
            case op_dconst_1: {
                JDouble * dval = new JDouble;
                dval->val = 1.0;
                currentFrame->stack.push(dval);
            }break;
            case op_bipush: {
                u1 byte = ext.code[++op];
                JInt * b = new JInt;
                b->val = byte;
                currentFrame->stack.push(b);
            }break;
            case op_sipush: {
                u2 byte = u2index(ext.code, op);
                JInt * b = new JInt;
                b->val = byte;
                currentFrame->stack.push(b);
            }break;
            case op_ldc: {
                u1 index = ext.code[++op];
                loadConstantPoolItem2Stack(jc, static_cast<u2>(index));
            }break;
            case op_ldc_w: {
                u2 index = u2index(ext.code, op);
                loadConstantPoolItem2Stack(jc, index);
            }break;
            case op_ldc2_w: {
                u2 index = u2index(ext.code, op);
                if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Double)) {
                    auto val = dynamic_cast<CONSTANT_Double*>(jc->raw.constPoolInfo[index])->val;
                    JDouble * dval = new JDouble;
                    dval->val = val;
                    currentFrame->stack.push(dval);
                }else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Long)) {
                    auto val = dynamic_cast<CONSTANT_Long*>(jc->raw.constPoolInfo[index])->val;
                    JLong * lval = new JLong;
                    lval->val = val;
                    currentFrame->stack.push(lval);
                }
                else {
                    throw std::runtime_error("invalid symbolic reference index on constant pool");
                }
            }break;
            case op_iload: {
                u1 index = ext.code[++op];
                load2Stack<JInt>(index);
            }break;
            case op_lload: {
                u1 index = ext.code[++op];
                load2Stack<JLong>(index);
            }break;
            case op_fload: {
                u1 index = ext.code[++op];
                load2Stack<JFloat>(index);
            }break;
            case op_dload: {
                u1 index = ext.code[++op];
                load2Stack<JDouble>(index);
            }break;
            case op_aload: {
                u1 index = ext.code[++op];
                load2Stack<JRef>(index);
            }break;
            case op_iload_0: {
                load2Stack<JInt>(0);
            }break;
            case op_iload_1: {
                load2Stack<JInt>(1);
            }break;
            case op_iload_2: {
                load2Stack<JInt>(2);
            }break;
            case op_iload_3: {
                load2Stack<JInt>(3);
            }break;
            case op_lload_0: {
                load2Stack<JLong>(0);
            }break;
            case op_lload_1: {
                load2Stack<JLong>(1);
            }break;
            case op_lload_2: {
                load2Stack<JLong>(2);
            }break;
            case op_lload_3: {
                load2Stack<JLong>(3);
            }break;
            case op_fload_0: {
                load2Stack<JFloat>(0);
            }break;
            case op_fload_1: {
                load2Stack<JFloat>(1);
            }break;
            case op_fload_2: {
                load2Stack<JFloat>(2);
            }break;
            case op_fload_3: {
                load2Stack<JFloat>(3);
            }break;
            case op_dload_0: {
                load2Stack<JDouble>(0);
            }break;
            case op_dload_1: {
                load2Stack<JDouble>(1);
            }break;
            case op_dload_2: {
                load2Stack<JDouble>(2);
            }break;
            case op_dload_3: {
                load2Stack<JDouble>(3);
            }break;
            case op_aload_0: {
                load2Stack<JRef>(0);
            }break;
            case op_aload_1: {
                load2Stack<JRef>(1);
            }break;
            case op_aload_2: {
                load2Stack<JRef>(2);
            }break;
            case op_aload_3: {
                load2Stack<JRef>(3);
            }break;
#if __cplusplus >= 201103L
            [[fallthrough]]
#endif
            case op_saload:
            case op_caload:
            case op_baload:
            case op_iaload: {
                loadArrayItem2Stack<JInt>();
            }break;
            case op_laload: {
                loadArrayItem2Stack<JLong>();
            }break;
            case op_faload: {
                loadArrayItem2Stack<JFloat>();
            }break;
            case op_daload: {
                loadArrayItem2Stack<JDouble>();
            }break;
            case op_aaload: {
                loadArrayItem2Stack<JRef>();
            }break;
            case op_istore: {
                u1 index = ext.code[++op];
                store2Local<JInt>(index);
            }break;
            case op_lstore: {
                u1 index = ext.code[++op];
                store2Local<JLong>(index);
            }break;
            case op_fstore: {
                u1 index = ext.code[++op];
                store2Local<JFloat>(index);
            }break;
            case op_dstore: {
                u1 index = ext.code[++op];
                store2Local<JDouble>(index);
            }break;
            case op_astore: {
                u1 index = ext.code[++op];
                store2Local<JRef>(index);
            }break;
            case op_istore_0: {
                store2Local<JInt>(0);
            }break;
            case op_istore_1: {
                store2Local<JInt>(1);
            }break;
            case op_istore_2: {
                store2Local<JInt>(2);
            }break;
            case op_istore_3: {
                store2Local<JInt>(3);
            }break;
            case op_lstore_0: {
                store2Local<JLong>(0);
            }break;
            case op_lstore_1: {
                store2Local<JLong>(1);
            }break;
            case op_lstore_2: {
                store2Local<JLong>(2);
            }break;
            case op_lstore_3: {
                store2Local<JLong>(3);
            }break;
            case op_fstore_0: {
                store2Local<JFloat>(0);
            }break;
            case op_fstore_1: {
                store2Local<JFloat>(1);
            }break;
            case op_fstore_2: {
                store2Local<JFloat>(2);
            }break;
            case op_fstore_3: {
                store2Local<JFloat>(3);
            }break;
            case op_dstore_0: {
                store2Local<JDouble>(0);
            }break;
            case op_dstore_1: {
                store2Local<JDouble>(1);
            }break;
            case op_dstore_2: {
                store2Local<JDouble>(2);
            }break;
            case op_dstore_3: {
                store2Local<JDouble>(3);
            }break;
            case op_astore_0: {
                store2Local<JRef>(0);
            }break;
            case op_astore_1: {
                store2Local<JRef>(1);
            }break;
            case op_astore_2: {
                store2Local<JRef>(2);
            }break;
            case op_astore_3: {
                store2Local<JRef>(3);
            }break;
            case op_iastore: {
                storeArrayItem<JInt>();
            }break;
            case op_lastore: {
                storeArrayItem<JLong>();
            }break;
            case op_fastore: {
                storeArrayItem<JFloat>();
            }break;
            case op_dastore: {
                storeArrayItem<JDouble>();
            }break;
            case op_aastore: {
                storeArrayItem<JRef>();
            }break;
            case op_bastore: {
                JInt * value = (JInt*)currentFrame->stack.top();
                value->val = static_cast<int8_t>(value->val);
                currentFrame->stack.pop();
                JInt * index = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JArray * arrayref = (JArray*)currentFrame->stack.top();
                if (arrayref == nullptr) {
                    throw std::runtime_error("null pointer");
                }
                if (index->val > arrayref->length || index->val < 0) {
                    throw std::runtime_error("array index out of bounds");
                }
                currentFrame->stack.pop();
                yrt.jheap->putArrayItem(*arrayref, index->val, value);

                delete index;
            }break;
#if __cplusplus >= 201103L
                [[fallthrough]]
#endif
            case op_sastore:
            case op_castore: {
                JInt * value = (JInt*)currentFrame->stack.top();
                value->val = static_cast<int16_t>(value->val);
                currentFrame->stack.pop();
                JInt * index = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JArray * arrayref = (JArray*)currentFrame->stack.top();
                if (arrayref == nullptr) {
                    throw std::runtime_error("null pointer");
                }
                if (index->val > arrayref->length || index->val < 0) {
                    throw std::runtime_error("array index out of bounds");
                }
                currentFrame->stack.pop();
                yrt.jheap->putArrayItem(*arrayref, index->val, value);

                delete index;
            }break;
            case op_pop: {
                delete currentStackPop<JType>();
            }break;
            case op_pop2: {
                delete currentStackPop<JType>();
                delete currentStackPop<JType>();
            }break;
            case op_dup: {
                JType * value = currentFrame->stack.top();
                currentFrame->stack.pop();

                assert(typeid(*value)!= typeid(JLong) && typeid(*value)!= typeid(JDouble));
                currentFrame->stack.push(value);
                currentFrame->stack.push(duplicateValue(value));
            }break;
            case op_dup_x1: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();

                assert(IS_COMPUTATIONAL_TYPE_1(value1));
                assert(IS_COMPUTATIONAL_TYPE_1(value2));

                currentFrame->stack.push(duplicateValue(value1));
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }break;
            case op_dup_x2: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value3 = currentFrame->stack.top();
                currentFrame->stack.pop();

                if(IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2) && IS_COMPUTATIONAL_TYPE_1(value3)){
                    // use structure 1
                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value3);
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else if(IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_2(value2)){
                    //use structure 2
                    currentFrame->stack.push(value3);

                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_dup2: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();

                if(IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2)){
                    // use structure 1
                    currentFrame->stack.push(duplicateValue(value2));
                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else if(IS_COMPUTATIONAL_TYPE_2(value1)){
                    //use structure 2
                    currentFrame->stack.push(value2);

                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value1);
                }else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_dup2_x1: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value3 = currentFrame->stack.top();
                currentFrame->stack.pop();

                if(IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2) && IS_COMPUTATIONAL_TYPE_1(value3)){
                    // use structure 1
                    currentFrame->stack.push(duplicateValue(value2));
                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value3);
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else if(IS_COMPUTATIONAL_TYPE_2(value1) && IS_COMPUTATIONAL_TYPE_1(value2)){
                    //use structure 2
                    currentFrame->stack.push(value3);

                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_dup2_x2: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value3 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value4 = currentFrame->stack.top();
                currentFrame->stack.pop();
                if(IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2)
                   && IS_COMPUTATIONAL_TYPE_1(value3) && IS_COMPUTATIONAL_TYPE_1(value4)){
                    // use structure 1
                    currentFrame->stack.push(duplicateValue(value2));
                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value4);
                    currentFrame->stack.push(value3);
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else if(IS_COMPUTATIONAL_TYPE_2(value1) && IS_COMPUTATIONAL_TYPE_1(value2)&& IS_COMPUTATIONAL_TYPE_1(value3)){
                    //use structure 2
                    currentFrame->stack.push(value4);

                    currentFrame->stack.push(duplicateValue(value1));
                    currentFrame->stack.push(value4);
                    currentFrame->stack.push(value2);
                    currentFrame->stack.push(value1);
                }else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_swap: {
                JType * value1 = currentFrame->stack.top();
                currentFrame->stack.pop();
                JType * value2 = currentFrame->stack.top();
                currentFrame->stack.pop();

                assert(IS_COMPUTATIONAL_TYPE_1(value1));
                assert(IS_COMPUTATIONAL_TYPE_1(value2));
                if(typeid(*value1) == typeid(JInt) && typeid(*value2) == typeid(JInt)){
                    std::swap(value1, value2);
                }else if(typeid(*value1) == typeid(JInt) && typeid(*value2) == typeid(JFloat)){
                    const int32_t temp = dynamic_cast<JInt*>(value1)->val;
                    dynamic_cast<JInt*>(value1)->val = static_cast<int32_t>(dynamic_cast<JFloat*>(value2)->val);
                    dynamic_cast<JFloat*>(value2)->val = static_cast<float>(temp);
                }else if(typeid(*value1) == typeid(JFloat) && typeid(*value2) == typeid(JInt)){
                    const float temp = dynamic_cast<JFloat*>(value1)->val;
                    dynamic_cast<JFloat*>(value1)->val = static_cast<int32_t>(dynamic_cast<JInt*>(value2)->val);
                    dynamic_cast<JInt*>(value2)->val = static_cast<int32_t>(temp);
                }else if(typeid(*value1) == typeid(JFloat) && typeid(*value2) == typeid(JFloat)){
                    std::swap(value1, value2);
                }else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_iadd: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val + value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ladd: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val + value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_fadd: {
                JFloat * value2 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JFloat * value1 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JFloat * result = new JFloat;
                result->val = value1->val + value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_dadd: {
                JDouble * value2 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JDouble * value1 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JDouble * result = new JDouble;
                result->val = value1->val + value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_isub: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val - value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_lsub: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val - value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_fsub: {
                JFloat * value2 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JFloat * value1 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JFloat * result = new JFloat;
                result->val = value1->val - value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_dsub: {
                JDouble * value2 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JDouble * value1 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JDouble * result = new JDouble;
                result->val = value1->val - value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_imul: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_lmul: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_fmul: {
                JFloat * value2 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JFloat * value1 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JFloat * result = new JFloat;
                result->val = value1->val * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_dmul: {
                JDouble * value2 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JDouble * value1 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JDouble * result = new JDouble;
                result->val = value1->val * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_idiv: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val / value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ldiv: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val / value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_fdiv: {
                JFloat * value2 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JFloat * value1 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JFloat * result = new JFloat;
                result->val = value1->val / value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ddiv: {
                JDouble * value2 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JDouble * value1 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JDouble * result = new JDouble;
                result->val = value1->val / value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_irem: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val - (value1->val/value2->val) * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_lrem: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val - (value1->val/value2->val) * value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_frem: {
                JFloat * value2 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JFloat * value1 = dynamic_cast<JFloat *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JFloat * result = new JFloat;
                result->val = std::fmod(value1->val,value2->val);
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_drem: {
                JDouble * value2 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JDouble * value1 = dynamic_cast<JDouble *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JDouble * result = new JDouble;
                result->val = std::fmod(value1->val,value2->val);
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ineg: {
                JInt * ival = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                ival->val = -ival->val;
                currentFrame->stack.push(ival);
            }break;
            case op_lneg: {
                JLong * lval = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                lval->val = -lval->val;
                currentFrame->stack.push(lval);
            }break;
            case op_fneg: {
                JFloat * fval = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                fval->val = -fval->val;
                currentFrame->stack.push(fval);
            }break;
            case op_dneg: {
                JDouble * dval = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                dval->val = -dval->val;
                currentFrame->stack.push(dval);
            }break;
            case op_ishl: {
                JInt *value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt *value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt * result = new JInt;
                result->val = value1->val * std::pow(2, value2->val & 0x1f);
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_lshl: {
                JInt * value2 = (JInt *)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong * value1 = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong * result = new JLong;
                result->val = value1->val * std::pow(2, value2->val & 0x3f);
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_ishr: {
                JInt *value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt *value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt * result = new JInt;
                result->val = std::floor(value1->val / std::pow(2, value2->val & 0x1f));
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_lshr: {
                JInt * value2 = (JInt *)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong * value1 = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong * result = new JLong;
                result->val = std::floor(value1->val / std::pow(2, value2->val & 0x3f));
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_iushr: {
                JInt *value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt *value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JInt * result = new JInt;
                if (value1->val > 0) {
                    result->val = value1->val >> (value2->val & 0x1f);
                }
                else if (value1->val < 0) {
                    result->val = (value1->val >> (value2->val & 0x1f)) + (2 << ~(value2->val & 0x1f));
                }
                else {
                    throw std::runtime_error("0 is not handled");
                }
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_lushr: {
                JInt *value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong *value1 = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                JLong * result = new JLong;
                if (value1->val > 0) {
                    result->val = value1->val >> (value2->val & 0x3f);
                }
                else if (value1->val < 0) {
                    result->val = (value1->val >> (value2->val & 0x1f)) + (2L << ~(value2->val & 0x3f));
                }
                else {
                    throw std::runtime_error("0 is not handled");
                }
                currentFrame->stack.push(result);

                delete value2;
                delete value1;
            }break;
            case op_iand: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val & value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_land: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val & value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ior: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val | value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_lor: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val | value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_ixor: {
                JInt * value2 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JInt * value1 = dynamic_cast<JInt *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JInt * result = new JInt;
                result->val = value1->val & value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_lxor: {
                JLong * value2 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();
                JLong * value1 = dynamic_cast<JLong *>(currentFrame->stack.top());
                currentFrame->stack.pop();

                JLong * result = new JLong;
                result->val = value1->val ^ value2->val;
                currentFrame->stack.push(result);

                delete value1;
                delete value2;
            }break;
            case op_iinc: {
                const u1 index = ext.code[++op];
                const int8_t count = ext.code[++op];
                int32_t extendedCount = count;
                if(IS_JINT(currentFrame->locals[index])){
                    dynamic_cast<JInt*>(currentFrame->locals[index])->val += extendedCount;
                }else if(IS_JLong(currentFrame->locals[index])){
                    dynamic_cast<JLong*>(currentFrame->locals[index])->val += extendedCount;
                }else if(IS_JFloat(currentFrame->locals[index])){
                    dynamic_cast<JFloat*>(currentFrame->locals[index])->val += extendedCount;
                }else if(IS_JDouble(currentFrame->locals[index])){
                    dynamic_cast<JDouble*>(currentFrame->locals[index])->val += extendedCount;
                } else{
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_i2l: {
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JLong;
                result->val = (int64_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_i2f: {
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JFloat;
                result->val = (float)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_i2d: {
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JDouble;
                result->val = (double)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_l2i: {
                auto * value = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JInt;
                result->val = (int32_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_l2f: {
                auto * value = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JFloat;
                result->val = (float)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_l2d: {
                auto * value = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JDouble;
                result->val = (double)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_f2i: {
                auto * value = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JInt;
                result->val = (int32_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_f2l: {
                auto * value = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JLong;
                result->val = (int64_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_f2d: {
                auto * value = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JDouble;
                result->val = (double)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_d2i: {
                auto * value = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JInt;
                result->val = (int32_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_d2l: {
                auto * value = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JLong;
                result->val = (int64_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_d2f: {
                auto * value = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JFloat;
                result->val = (float)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;

#if __cplusplus >= 201103L
                [[fallthrough]]
#endif
            case op_i2c:
            case op_i2b: {
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JInt;
                result->val = (int8_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_i2s: {
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * result = new JInt;
                result->val = (int16_t)(value->val);
                currentFrame->stack.push(result);
                delete value;
            }break;
            case op_lcmp: {
                auto * value2 = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JLong*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val > value2->val) {
                    auto * result = new JInt(1);
                    currentFrame->stack.push(result);
                }
                else if (value1->val == value2->val) {
                    auto * result = new JInt(0);
                    currentFrame->stack.push(result);
                }
                else {
                    auto * result = new JInt(-1);
                    currentFrame->stack.push(result);
                }
                delete value1;
                delete value2;
            }break;
#if __cplusplus >= 201103L
            [[fallthrough]]
#endif
            case op_fcmpg:
            case op_fcmpl: {
                auto * value2 = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JFloat*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val > value2->val) {
                    auto * result = new JInt(1);
                    currentFrame->stack.push(result);
                }
                else if (std::abs(value1->val - value2->val) < 0.000001) {
                    auto * result = new JInt(0);
                    currentFrame->stack.push(result);
                }
                else {
                    auto * result = new JInt(-1);
                    currentFrame->stack.push(result);
                }
                delete value1;
                delete value2;
            }break;
#if __cplusplus >= 201103L
                [[fallthrough]]
#endif
            case op_dcmpl:
            case op_dcmpg: {
                auto * value2 = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JDouble*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val > value2->val) {
                    auto * result = new JInt(1);
                    currentFrame->stack.push(result);
                }
                else if (std::abs(value1->val - value2->val) < 0.000000000001) {
                    auto * result = new JInt(0);
                    currentFrame->stack.push(result);
                }
                else {
                    auto * result = new JInt(-1);
                    currentFrame->stack.push(result);
                }
                delete value1;
                delete value2;
            }break;
            case op_ifeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val == 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_ifne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val != 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_iflt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val < 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_ifge: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val >= 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_ifgt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val > 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_ifle: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value->val <= 0) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value;
            }break;
            case op_if_icmpeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val == value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_icmpne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val != value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_icmplt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val < value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_icmpge: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val >= value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_icmpgt: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val > value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_icmple: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->val <= value2->val) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_acmpeq: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->offset == value2->offset && value1->jc == value2->jc) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_if_acmpne: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                auto * value2 = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto * value1 = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value1->offset != value2->offset || value1->jc != value2->jc) {
                    op = currentOffset + branchindex;
                    yrt.eip = op;
                }
                delete value1;
                delete value2;
            }break;
            case op_goto: {
                u4 currentOffset = op - 1;
                int16_t branchindex = u2index(ext.code, op);
                op = currentOffset + branchindex;
                yrt.eip = op;
            }break;
            case op_jsr: {
                throw std::runtime_error("unsupported opcode [jsr]");
            }break;
            case op_ret: {
                throw std::runtime_error("unsupported opcode [ret]");
            }break;
            case op_tableswitch: {
                u4 currentOffset = op - 1;
                op++;
                op++;
                op++;   //3 bytes padding
                int32_t defaultIndex = u4index(ext.code, op);
                int32_t low = u4index(ext.code, op);
                int32_t high = u4index(ext.code, op);
                std::vector<int32_t> jumpOffset;
                FOR_EACH(i, high - low + 1) {
                    jumpOffset.push_back(u4index(ext.code, op));
                }

                auto * index = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (index->val < low || index->val > high) {
                    op = currentOffset + defaultIndex;
                    yrt.eip = op;
                }
                else {
                    op = currentOffset + jumpOffset[index->val - low];
                    yrt.eip = op;
                }
                delete index;
            }break;
            case op_lookupswitch: {
                u4 currentOffset = op - 1;
                op++;
                op++;
                op++;   //3 bytes padding
                int32_t defaultIndex = u4index(ext.code, op);
                int32_t npair = u4index(ext.code, op);
                std::map<int32_t, int32_t> matchOffset;
                FOR_EACH(i, npair) {
                    matchOffset.insert(std::make_pair(u4index(ext.code, op), u4index(ext.code, op)));
                }
                auto * key = (JInt*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto res = matchOffset.find(key->val);
                if (res != matchOffset.end()) {
                    op = currentOffset + (*res).second;
                    yrt.eip = op;
                }
                else {
                    op = currentOffset + defaultIndex;
                    yrt.eip = op;
                }
                delete key;
            }break;
            case op_ireturn: {
                return flowReturn<JInt>();
            }break;
            case op_lreturn: {
                return flowReturn<JLong>();
            }break;
            case op_freturn: {
                return flowReturn<JFloat>();
            }break;
            case op_dreturn: {
                return flowReturn<JDouble>();
            }break;
            case op_areturn: {
                return flowReturn<JType>();
            }break;
            case op_return: {
                return nullptr;
            }break;
            case op_getstatic: {
                u2 index = u2index(ext.code, op);
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                JType * field = duplicateValue(getStaticField(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef)));
                currentFrame->stack.push(field);
            }break;
            case op_putstatic: {
                u2 index = u2index(ext.code, op);
                JType * value = currentFrame->stack.top();
                currentFrame->stack.pop();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                putStaticField(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef), value);
            }break;
            case op_getfield: {
                u2 index = u2index(ext.code, op);
                JObject * objectref = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                JType * field = duplicateValue(getInstanceField(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef),
                    objectref, 0));
                currentFrame->stack.push(field);

                delete objectref;
            }break;
            case op_putfield: {
                u2 index = u2index(ext.code, op);
                JType * value = currentFrame->stack.top();
                currentFrame->stack.pop();
                JObject * objectref = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                auto symbolicRef = parseFieldSymbolicReference(jc, index);
                putInstanceField(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef),
                    objectref, value, 0);

                delete objectref;
            }break;
//////////////////////////////////////////////////////
// bug region start
// write symbolic reference inline function 
/////////////////////////////////////////////////////
            case op_invokevirtual: {
                const u2 index = u2index(ext.code, op);
                assert(typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref));
                
                auto symbolicRef = parseMethodSymbolicReference(jc, index);
                
                if (strcmp(std::get<1>(symbolicRef), "<init>") == 0) {
                    std::runtime_error("invoking method should not be instance initialization method\n");
                }
                if (!IS_SIGNATURE_POLYMORPHIC_METHOD((char*)std::get<0>(symbolicRef)->getClassName(), std::get<1>(symbolicRef))) {
                    invokeVirtual(std::get<1>(symbolicRef), std::get<2>(symbolicRef));
                }
                else {
                    //TODO:TO BE IMPLEMENTED
                }
                
            }break;
            case op_invokespecial: {
                u2 index = u2index(ext.code, op);
                std::tuple<JavaClass*, const char*, const char*> symbolicRef;

                if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                    symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
                }
                else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref)) {
                    symbolicRef = parseMethodSymbolicReference(jc, index);
                }
                else {
                    SHOULD_NOT_REACH_HERE
                }

                // If all of the following are true, let C be the direct superclass of the
                // current class :
                JavaClass * symbolicRefClass = std::get<0>(symbolicRef);
                if (strcmp("<init>", std::get<1>(symbolicRef)) != 0) {
                    if (!IS_CLASS_INTERFACE(symbolicRefClass->raw.accessFlags)) {
                        if (strcmp((char*)symbolicRefClass->getClassName(), (char*)jc->getSuperClassName()) == 0) {
                            if (IS_CLASS_SUPER(jc->raw.accessFlags)) {
                                invokeSpecial(yrt.ma->findJavaClass((char*)jc->getSuperClassName()), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
                                break;
                            }
                        }
                    }
                }
                // Otherwise let C be the symbolic reference class
                invokeSpecial(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
            }break;
            case op_invokestatic: {
                // Invoke a class (static) method
                u2 index = u2index(ext.code, op);
                
                if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                    auto symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
                    invokeStatic(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
                }
                else if(typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref)) {
                    auto symbolicRef = parseMethodSymbolicReference(jc, index);
                    invokeStatic(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
                }
                else {
                    SHOULD_NOT_REACH_HERE
                }
            }break;
            case op_invokeinterface: {
                u2 index = u2index(ext.code, op);
                ++op;                       // read count and discard
                ++op;                       //opcode padding 0;

                if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                    auto symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
                    invokeInterface(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
                }
            }break;
            case op_invokedynamic: {
                throw std::runtime_error("unsupported opcode [invokedynamic]");
            }break;
//////////////////////////////////////////////////////
// bug region end
/////////////////////////////////////////////////////
            case op_new: {
                u2 index = u2index(ext.code, op);
                JObject * objectref = execNew(jc, index);
                currentFrame->stack.push(objectref);
            }break;
            case op_newarray: {
                u1 atype = ext.code[++op];
                JInt * count = dynamic_cast<JInt*>(currentFrame->stack.top());
                currentFrame->stack.pop();

                if (count->val < 0) {
                    throw std::runtime_error("negative array size");
                }
                JArray * arrayref = yrt.jheap->createPODArray(atype, count->val);

                currentFrame->stack.push(arrayref);
                delete count;
            }break;
            case op_anewarray: {
                u2 index = u2index(ext.code, op);
                JInt * count = dynamic_cast<JInt*>(currentFrame->stack.top());
                currentFrame->stack.pop();

                if (count->val < 0) {
                    throw std::runtime_error("negative array size");
                }
                JArray * arrayref = yrt.jheap->createObjectArray(*jc, count->val);

                currentFrame->stack.push(arrayref);
                delete count;
            }break;
            case op_arraylength: {
                JArray * arrayref = dynamic_cast<JArray*>(currentFrame->stack.top());
                currentFrame->stack.pop();

                if (arrayref == nullptr) {
                    throw std::runtime_error("null pointer\n");
                }
                JInt * length = new JInt;
                length->val = arrayref->length;
                currentFrame->stack.push(length);

                delete arrayref;
            }break;
            case op_athrow: {
                throw std::runtime_error("unsupported opcode [athrow]");
            }break;
            case op_checkcast: {
                throw std::runtime_error("unsupported opcode [checkcast]");
            }break;
            case op_instanceof: {
                throw std::runtime_error("unsupported opcode [instanceof]");
            }break;
            case op_monitorenter: {
                throw std::runtime_error("unsupported opcode [monitorenter]");
            }break;
            case op_monitorexit: {
                throw std::runtime_error("unsupported opcode [monitorexit]");
            }break;
            case op_wide: {
                throw std::runtime_error("unsupported opcode [wide]");
            }break;
            case op_multianewarray: {
                throw std::runtime_error("unsupported opcode [multianewarray]");
            }break;
            case op_ifnull: {
                u4 currentOffset = op - 1;
                int16_t branchIndex = u2index(ext.code, op);
                JObject * value = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value == nullptr) {
                    delete value;
                    op = currentOffset + branchIndex;
                }
            }break;
            case op_ifnonnull: {
                u4 currentOffset = op - 1;
                int16_t branchIndex = u2index(ext.code, op);
                JObject * value = (JObject*)currentFrame->stack.top();
                currentFrame->stack.pop();
                if (value != nullptr) {
                    delete value;
                    op = currentOffset + branchIndex;
                }
            }break;
            case op_goto_w: {
                u4 currentOffset = op - 1;
                int32_t branchIndex = u4index(ext.code, op);
                op = currentOffset + branchIndex;
            }break;
            case op_jsr_w: {
                throw std::runtime_error("unsupported opcode [jsr_w]");
            }break;
            case op_breakpoint: {
                throw std::runtime_error("reserved opcode [breakpoint]. Are you doing some hacking?");
            }break;
            case op_impdep1: {
                throw std::runtime_error("reserved opcode [impdep1]. Are you doing some hacking?");
            }break;
            case op_impdep2: {
                throw std::runtime_error("reserved opcode [impdep2]. Are you doing some hacking?");
            }break;
            default:
                std::cerr << "Invalid opcode detected!\n";
        }
    }
    return nullptr;
}

void CodeExecution::loadConstantPoolItem2Stack(const JavaClass *jc, u2 index) {
    if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Integer)) {
        auto val = dynamic_cast<CONSTANT_Integer*>(jc->raw.constPoolInfo[index])->val;
        JInt * ival = new JInt;
        ival->val = val;
        currentFrame->stack.push(ival);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Float)) {
        auto val = dynamic_cast<CONSTANT_Float*>(jc->raw.constPoolInfo[index])->val;
        JFloat * fval = new JFloat;
        fval->val = val;
        currentFrame->stack.push(fval);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_String)) {
        auto val = (char*)jc->getString(dynamic_cast<CONSTANT_String*>(jc->raw.constPoolInfo[index])->stringIndex);
        JObject * str = yrt.jheap->createObject(*yrt.ma->loadClassIfAbsent("java/lang/String"));
        JArray * value = yrt.jheap->createCharArray(val, strlen(val));
        // put string  into str's field; according the source file of java.lang.Object, we know that 
        // its first field was used to store chars
        yrt.jheap->putObjectField(*str, 0, value);
        currentFrame->stack.push(str);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Class)) {
        throw std::runtime_error("unsupport region");
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_MethodType)) {
        throw std::runtime_error("unsupport region");
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_MethodHandle)) {
        throw std::runtime_error("unsupport region");
    }
    else {
        throw std::runtime_error("invalid symbolic reference index on constant pool");
    }
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseFieldSymbolicReference(const JavaClass * jc, u2 index) {
    const char * symbolicReferenceFieldName = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char * symbolicReferenceFieldDescriptor = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->descriptorIndex);

    JavaClass * symbolicReferenceClass = yrt.ma->loadClassIfAbsent((char*)jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->classIndex])->nameIndex));

    yrt.ma->linkClassIfAbsent((char*)symbolicReferenceClass->getClassName());

    return std::make_tuple(symbolicReferenceClass,
        symbolicReferenceFieldName, 
        symbolicReferenceFieldDescriptor);
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseInterfaceMethodSymbolicReference(const JavaClass * jc, u2 index){
    const char * symbolicReferenceInterfaceMethodName = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char * symbolicReferenceInterfaceMethodDescriptor = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->descriptorIndex);

    JavaClass * symbolicReferenceInterfaceMethodClass = yrt.ma->loadClassIfAbsent((char*)jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc->raw.constPoolInfo[index])->classIndex])->nameIndex));
    yrt.ma->linkClassIfAbsent((char*)symbolicReferenceInterfaceMethodClass->getClassName());

    return std::make_tuple(symbolicReferenceInterfaceMethodClass,
        symbolicReferenceInterfaceMethodName, 
        symbolicReferenceInterfaceMethodDescriptor);
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseMethodSymbolicReference(const JavaClass * jc, u2 index) {
    const char * symbolicReferenceMethodName = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char * symbolicReferenceMethodDescriptor = (const char*)jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->descriptorIndex);

    JavaClass * symbolicReferenceMethodClass = yrt.ma->loadClassIfAbsent((char*)jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index])->classIndex])->nameIndex));
    yrt.ma->linkClassIfAbsent((char*)symbolicReferenceMethodClass->getClassName());

    return std::make_tuple(symbolicReferenceMethodClass,
        symbolicReferenceMethodName,
        symbolicReferenceMethodDescriptor);
}

JType * CodeExecution::getStaticField(JavaClass * parsedJc,const char * fieldName, const char * fieldDescriptor) {
    yrt.ma->linkClassIfAbsent((char*)parsedJc->getClassName());
    yrt.ma->initClassIfAbsent(*this, (char*)parsedJc->getClassName());

    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            const char * n = (char*)parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char * d = (char*)parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor)==0) {
                return parsedJc->sfield.find(i)->second;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        return getStaticField(yrt.ma->findJavaClass((char*)parsedJc->getSuperClassName()), fieldName, fieldDescriptor);
    }
    return nullptr;
}

void CodeExecution::putStaticField(JavaClass * parsedJc, const char * fieldName, const char * fieldDescriptor, JType * value) {
    yrt.ma->linkClassIfAbsent((char*)parsedJc->getClassName());
    yrt.ma->initClassIfAbsent(*this, (char*)parsedJc->getClassName());

    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            const char * n = (char*)parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char * d = (char*)parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                parsedJc->sfield.find(i)->second = value;
                return;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        putStaticField(yrt.ma->findJavaClass((char*)parsedJc->getSuperClassName()), fieldName, fieldDescriptor, value);
    }
}

JType * CodeExecution::getInstanceField(JavaClass * parsedJc,const char * fieldName, const char * fieldDescriptor, 
    JObject * object, size_t offset) {
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const char * n = (char*)parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char * d = (char*)parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                 return yrt.jheap->objheap[object->offset].at(i + offset);
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        return getInstanceField(yrt.ma->findJavaClass((char*)parsedJc->getSuperClassName()), fieldName, fieldDescriptor,
            object, offset + howManyNonStaticFields);
    }
    return nullptr;
}

void CodeExecution::putInstanceField(JavaClass * parsedJc, const char * fieldName, const char * fieldDescriptor, 
    JObject * object, JType * value, size_t offset){
    size_t howManyNonStaticFields = 0;
    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (!IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            howManyNonStaticFields++;
            const char * n = (char*)parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char * d = (char*)parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                yrt.jheap->objheap[object->offset].at(offset + i) = value;
                return;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        putInstanceField(yrt.ma->findJavaClass((char*)parsedJc->getSuperClassName()), fieldName, fieldDescriptor, object, 
            value, offset + howManyNonStaticFields);
    }
}

JObject * CodeExecution::execNew(const JavaClass * jc, u2 index) {
    yrt.ma->linkClassIfAbsent((char*)const_cast<JavaClass*>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this, (char*)const_cast<JavaClass*>(jc)->getClassName());

    if (typeid(*jc->raw.constPoolInfo[index]) != typeid(CONSTANT_Class)) {
        throw std::runtime_error("operand index of new is not a class or interface\n");
    }
    char * className = (char *)jc->getString(dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[index])->nameIndex);
    JavaClass * newClass = yrt.ma->loadClassIfAbsent(className);
    return yrt.jheap->createObject(*newClass);
}

CodeExecution::CodeExtension CodeExecution::getCodeExtension(const MethodInfo * m) {
    CodeExtension ext{};
    if(!m){
        return ext;
    }

    FOR_EACH(i, m->attributeCount) {
        if (typeid(*m->attributes[i]) == typeid(ATTR_Code)) {
            ext.code = dynamic_cast<ATTR_Code*>(m->attributes[i])->code;
            ext.codeLength = ((ATTR_Code*)m->attributes[i])->codeLength;
            ext.maxLocal = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxLocals;
            ext.maxStack = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxStack;
            ext.exceptionTable = dynamic_cast<ATTR_Code*>(m->attributes[i])->exceptionTable;
            ext.valid = true;
            break;
        }
    }
    return ext;
}

std::pair<MethodInfo *, const JavaClass*> CodeExecution::findMethod(const JavaClass * jc, const char * methodName, const char * methodDescriptor) {
    // Find corresponding method at current object's class;
    FOR_EACH(i, jc->raw.methodsCount) {
        auto * methodInfo = jc->getMethod(methodName, methodDescriptor);
        if (methodInfo) {
            return std::make_pair(methodInfo, jc);
        }
    }
    // Find corresponding method at object's super class unless it's an instance of
    if (jc->raw.superClass != 0) {
        JavaClass * superClass = yrt.ma->loadClassIfAbsent((const char *)(jc->getSuperClassName()));
        auto methodPair = findMethod(jc, methodName, methodDescriptor);
        if (methodPair.first) {
            return methodPair;
        }
    }
    // Find corresponding method at object's all interfaces if at least one interface class existing
    if (jc->raw.interfacesCount>0) {
        FOR_EACH(eachInterface, jc->raw.interfacesCount) {
            const auto * interfaceName = (const char *)jc->getString(dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[jc->raw.interfaces[eachInterface]])->nameIndex);
            JavaClass * interfaceClass = yrt.ma->loadClassIfAbsent(interfaceName);
            auto * methodInfo = interfaceClass->getMethod(methodName, methodDescriptor);
            if (methodInfo && (!IS_METHOD_ABSTRACT(methodInfo->accessFlags)
                && !IS_METHOD_STATIC(methodInfo->accessFlags)
                && !IS_METHOD_PRIVATE(methodInfo->accessFlags))) {
                return std::make_pair(methodInfo, interfaceClass);
            }
            if (methodInfo && (!IS_METHOD_STATIC(methodInfo->accessFlags)
                && !IS_METHOD_PRIVATE(methodInfo->accessFlags))) {
                return std::make_pair(methodInfo, interfaceClass);
            }
        }
    }
    // Otherwise, failed to find corresponding method by given method name and method descriptor
    return std::make_pair(nullptr, nullptr);
}

void CodeExecution::invokeByName(JavaClass * jc,const char * methodName, const char * methodDescriptor) {
    const MethodInfo * m = jc->getMethod(methodName, methodDescriptor);
    CodeExtension ext = getCodeExtension(m);


    if (!ext.valid) {
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
        std::cout << "Method " << jc->getClassName() << "::" << methodName << "() not found!\n";
#endif
        return;
    }

#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for(int i=0;i<yrt.frames.size();i++){
        std::cout << "-";
    }
    std::cout <<"Execute " << jc->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    // Actual method calling routine
    Frame * frame = new Frame;
    frame->locals.resize(ext.maxLocal);
    yrt.frames.push(frame);
    currentFrame = yrt.frames.top();

    JType * returnValue{};
    if (IS_METHOD_NATIVE(m->accessFlags)) {
        invokeNativeMethod((char*)jc->getClassName(), methodName, methodDescriptor);
    }else{
        returnValue = execCode(jc, ext);
    }
    popFrame();
    if (yrt.frames.empty()) {
        currentFrame = nullptr;
    }
    else {
        currentFrame = yrt.frames.top();
    }
    yrt.eip = 0;
}

void CodeExecution::invokeInterface(const JavaClass * jc, const char * methodName, const char * methodDescriptor) {
    // Invoke interface method

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<yrt.frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    if (invokingMethod.first == nullptr) {
        throw std::runtime_error("no such method existed");
    }

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);
    Frame * frame = new Frame;
    for (int64_t i = (int64_t)parameter.size() - 1; i >= 0; i--) {
        if (parameter[i] == T_INT || parameter[i] == T_BOOLEAN ||
            parameter[i] == T_CHAR || parameter[i] == T_BYTE || parameter[i] == T_SHORT) {
            auto * v = (JInt*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_FLOAT) {
            auto * v = (JFloat*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_DOUBLE) {
            auto * v = (JDouble*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_LONG) {
            auto * v = (JLong*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_ARRAY) {
            auto * v = (JArray*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_OBJECT) {
            auto * v = (JObject*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else {
            SHOULD_NOT_REACH_HERE;
        }
    }
    auto * objectref = (JObject*)currentFrame->stack.top();
    currentFrame->stack.pop();
    frame->locals.push_front(objectref);
    CodeExtension ext = getCodeExtension(invokingMethod.first);
    frame->locals.resize(ext.maxLocal);
    yrt.frames.push(frame);
    this->currentFrame = frame;
    
    JType * returnValue{};
    if(IS_METHOD_NATIVE(invokingMethod.first->accessFlags)){
        invokeNativeMethod((char*)const_cast<JavaClass*>(invokingMethod.second)->getClassName(), methodName, methodDescriptor);
    }else{
        returnValue = execCode(invokingMethod.second, ext);
    }

    popFrame();
    if (returnType != T_EXTRA_VOID)
        currentFrame->stack.push(returnValue);
}

void CodeExecution::invokeVirtual(const char * methodName, const char * methodDescriptor) {
    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);

    Frame * frame = new Frame;
    for (int64_t i = (int64_t)parameter.size() - 1; i >= 0; i--) {
        if (parameter[i] == T_INT || parameter[i] == T_BOOLEAN ||
            parameter[i] == T_CHAR || parameter[i] == T_BYTE || parameter[i] == T_SHORT) {
            auto * v = (JInt*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_FLOAT) {
            auto * v = (JFloat*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_DOUBLE) {
            auto * v = (JDouble*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_LONG) {
            auto * v = (JLong*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_ARRAY) {
            auto * v = (JArray*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_OBJECT) {
            auto * v = (JObject*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else {
            SHOULD_NOT_REACH_HERE;
        }
    }
    auto * objectref = (JObject*)currentFrame->stack.top();
    currentFrame->stack.pop();
    frame->locals.push_front(objectref);
    yrt.frames.push(frame);
    this->currentFrame = frame;

    auto invokingMethod = findMethod(objectref->jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<yrt.frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    JType * returnValue{};
    if (invokingMethod.first) {
        if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
            invokeNativeMethod((char*)const_cast<JavaClass*>(invokingMethod.second)->getClassName(),
                (char*)invokingMethod.second->getString(invokingMethod.first->nameIndex), 
                (char*)invokingMethod.second->getString(invokingMethod.first->descriptorIndex));
        }
        else {
            auto ext = getCodeExtension(invokingMethod.first);
            frame->locals.resize(ext.maxLocal);
            returnValue = execCode(invokingMethod.second, ext);
        }
    }
    else {
        throw std::runtime_error("can not find method to call");
    }

    popFrame();
    if (returnType != T_EXTRA_VOID)
        currentFrame->stack.push(returnValue);
}

void CodeExecution::invokeSpecial(const JavaClass * jc, const char * methodName, const char * methodDescriptor) {
    //  Invoke instance method; special handling for superclass, private,
    //  and instance initialization method invocations

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);

    Frame * frame = new Frame;
    for (int64_t i = (int64_t)parameter.size() - 1; i >= 0; i--) {
        if (parameter[i] == T_INT || parameter[i] == T_BOOLEAN ||
            parameter[i] == T_CHAR || parameter[i] == T_BYTE || parameter[i] == T_SHORT) {
            auto * v = (JInt*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_FLOAT) {
            auto * v = (JFloat*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_DOUBLE) {
            auto * v = (JDouble*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_LONG) {
            auto * v = (JLong*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_ARRAY) {
            auto * v = (JArray*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_OBJECT) {
            auto * v = (JObject*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else {
            SHOULD_NOT_REACH_HERE;
        }
    }
    auto * objectref = (JObject*)currentFrame->stack.top();
    currentFrame->stack.pop();
    frame->locals.push_front(objectref);
    yrt.frames.push(frame);
    this->currentFrame = frame;

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<yrt.frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    JType * returnValue{};
    if (invokingMethod.first) {
        if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
            invokeNativeMethod((char*)const_cast<JavaClass*>(invokingMethod.second)->getClassName(), 
                (char*)invokingMethod.second->getString(invokingMethod.first->nameIndex),
                (char*)invokingMethod.second->getString(invokingMethod.first->descriptorIndex));
        }
        else {
            auto ext = getCodeExtension(invokingMethod.first);
            frame->locals.resize(ext.maxLocal);
            returnValue = execCode(invokingMethod.second, ext);
        }
    }
    else if (IS_CLASS_INTERFACE(jc->raw.accessFlags)) {
        JavaClass * javaLangObjectClass = yrt.ma->findJavaClass("java/lang/Object");
        MethodInfo * javaLangObjectMethod = javaLangObjectClass->getMethod(methodName, methodDescriptor);
        if (javaLangObjectMethod && 
            IS_METHOD_PUBLIC(javaLangObjectMethod->accessFlags) && 
            !IS_METHOD_STATIC(javaLangObjectMethod->accessFlags)) {
            if (IS_METHOD_NATIVE(javaLangObjectMethod->accessFlags)) {
                invokeNativeMethod((char*)javaLangObjectClass->getClassName(),
                    (char*)javaLangObjectClass->getString(javaLangObjectMethod->nameIndex),
                    (char*)javaLangObjectClass->getString(javaLangObjectMethod->descriptorIndex));
            }
            else {
                auto ext = getCodeExtension(javaLangObjectMethod);
                frame->locals.resize(ext.maxLocal);
                returnValue = execCode(javaLangObjectClass, ext);
            }
        }
    }
    
    popFrame();
    if (returnType != T_EXTRA_VOID)
        currentFrame->stack.push(returnValue);
}

void CodeExecution::invokeStatic(const JavaClass * jc, const char * methodName, const char * methodDescriptor) {
    // Get instance method name and descriptor from CONSTANT_Methodref locating by index
    // and get interface method parameter and return value descriptor
    yrt.ma->linkClassIfAbsent((char*)const_cast<JavaClass*>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this, (char*)const_cast<JavaClass*>(jc)->getClassName());

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<yrt.frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif
    assert(IS_METHOD_STATIC(invokingMethod.first->accessFlags) == true);
    assert(IS_METHOD_ABSTRACT(invokingMethod.first->accessFlags) == false);
    assert(strcmp("<init>",methodName)!=0);

    Frame * frame = new Frame;
    auto ext = getCodeExtension(invokingMethod.first);
    frame->locals.resize(ext.maxLocal);

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);
    for (int64_t i = (int64_t)parameter.size() - 1; i >= 0; i--) {
        if (parameter[i] == T_INT || parameter[i] == T_BOOLEAN ||
            parameter[i] == T_CHAR || parameter[i] == T_BYTE || parameter[i] == T_SHORT) {
            auto * v = (JInt*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_FLOAT) {
            auto * v = (JFloat*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_DOUBLE) {
            auto * v = (JDouble*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_LONG) {
            auto * v = (JLong*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_ARRAY) {
            auto * v = (JArray*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_OBJECT) {
            auto * v = (JObject*)currentFrame->stack.top();
            currentFrame->stack.pop();
            frame->locals.push_front(v);
        }
        else {
            SHOULD_NOT_REACH_HERE;
        }
    }
    
    yrt.frames.push(frame);
    this->currentFrame = frame;

    JType * returnValue{};
    if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
        invokeNativeMethod((char*)const_cast<JavaClass*>(invokingMethod.second)->getClassName(),methodName, methodDescriptor);
    }
    else {
        returnValue = execCode(invokingMethod.second, ext);
    }
    popFrame();

    if (returnType != T_EXTRA_VOID)
        currentFrame->stack.push(returnValue);
}

void CodeExecution::invokeNativeMethod(const char * className, const char * methodName, const char * methodDescriptor) {
    std::string nativeMethod(className);
    nativeMethod.append(".");
    nativeMethod.append(methodName);
    nativeMethod.append(".");
    nativeMethod.append(methodDescriptor);
    if (yrt.nativeMethods.find(nativeMethod) != yrt.nativeMethods.end()) {
        ((*yrt.nativeMethods.find(nativeMethod)).second)(&yrt);
    }
}
