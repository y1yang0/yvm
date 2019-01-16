#ifndef YVM_OPTION_H
#define YVM_OPTION_H

//--------------------------------------------------------------------------------
// denote the default threshold value of garbage collector. GC was started when
// the memory allocation was beyond this value.
//--------------------------------------------------------------------------------
#define YVM_GC_THRESHOLD_VALUE (1024 * 1024 * 10)

//--------------------------------------------------------------------------------
// show new spawning thread name
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_THREAD_NAME

//--------------------------------------------------------------------------------
// show byte size of internal structures
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE

//--------------------------------------------------------------------------------
// how executing bytecode and current class of method, method name and method
// descriptor
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_SHOW_BYTECODE
#undef YVM_DEBUG_SHOW_EXEC_FLOW

//--------------------------------------------------------------------------------
// output *.class parsing result, which contains constant pool table, fields,
// methods, class access flag, interfaces, etc
//--------------------------------------------------------------------------------
#undef YVM_DEBUG_CLASS_FILE
#ifdef YVM_DEBUG_CLASS_FILE
#define YVM_DEBUG_SHOW_VERSION
#define YVM_DEBUG_SHOW_CONSTANT_POOL_TABLE
#define YVM_DEBUG_SHOW_INTERFACE
#define YVM_DEBUG_SHOW_CLASS_FIELD
#define YVM_DEBUG_SHOW_CLASS_ACCESS_FLAGS
#define YVM_DEBUG_SHOW_CLASS_METHOD
#define YVM_DEBUG_SHOW_CLASS_ATTRIBUTE
#endif

//--------------------------------------------------------------------------------
// to mark a gc safe point
//--------------------------------------------------------------------------------
#define GC_SAFE_POINT

#endif  // !YVM_OPTION_H
