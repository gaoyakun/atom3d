
/*  This header is referenced from within co-vage.lnt via the option:
	-header(co-vage.h)
    It is processed in this fashion to allow for C vs. C++ dependencies
 */
#if __cplusplus
#define __IBMCPP__ 300
#define __IBMC__ 0
#else
#define __IBMCPP__ 0
#define __IBMC__ 300
#endif
