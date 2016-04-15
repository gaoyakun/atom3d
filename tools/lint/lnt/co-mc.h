
// The following is designed to support the use of the asm() function
// which in turn provides support for in-line assembly.  The format of
// the asm() function is asm(type,string).  The result is an expression
// whose C type is given as first argument.

// this file can automatically be included within every module by
// use of the PC-lint option:
//
//                   -header(co-mc.h)


#define asm(type,string) ((type) __asm_func())
unsigned __asm_func(void);



