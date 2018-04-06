#ifndef YVM_OPTION_H
#define YVM_OPTION_H

/*
 * define to suspend console to see what going on this output
 */
#define YVM_DEBUG_SUSPEND_CONSOLE

/*
 * define to output byte size of yvm's internal structures
 */
#undef YVM_DEBUG_SHOW_SIZEOF_ALL_TYPE


/*
 * defint to show executing bytecode and current class of method, method name and
 * method descriptor
 */
#undef YVM_DEBUG_SHOW_BYTECODE
#undef  YVM_DEBUG_SHOW_EXEC_FLOW


/*
* define to output *.class parsing result, which contains constant pool table,
* fields, methods, class access flag, interfaces, etc
*/
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


#endif // !YVM_OPTION_H
