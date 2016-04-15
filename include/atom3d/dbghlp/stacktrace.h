/**	\file send_error.h
 *	堆栈跟踪支持
 *
 *	\author 高雅昆
 *	\addtogroup dbghlp
 *	@{
 */

#ifndef __ATOM3D_DBGHLP_STACKTRACE_H
#define __ATOM3D_DBGHLP_STACKTRACE_H

#include "basedefs.h"

#define ATOM_NO_STACK_TRACE

/**
	\def ATOM_STACK_TRACE(func)
	在函数体第一行使用此宏可使函数\a func支持堆栈跟踪
	\code
		void Foo (void)
		{
			ATOM_STACK_TRACE(Foo)
			// do some thing
		}
	\endcode
	开启堆栈跟踪对性能有一定影响，建议只对部分函数实现堆栈跟踪
	要禁用堆栈跟踪，在项目中定义ATOM_NO_STACK_TRACE
 */

#ifndef ATOM_NO_STACK_TRACE
# define ATOM_STACK_TRACE(func) ATOM_StackTrace autoStackTrace(#func)
# define ATOM_STACK_TRACE_EX(func, ...) ATOM_StackTrace autoStackTraceEx(#func, __VA_ARGS__)
#else
# define ATOM_STACK_TRACE(func)
# define ATOM_STACK_TRACE_EX(func, ...)
#endif

class ATOM_DBGHLP_API ATOM_StackTrace
{
public:
	ATOM_StackTrace (const char *functionName);
	~ATOM_StackTrace (void);
private:
	bool _added;
};

class ATOM_DBGHLP_API ATOM_StackTraceEx
{
public:
	ATOM_StackTraceEx (const char *functionName, ...);
	~ATOM_StackTraceEx (void);
private:
	bool _added;
};

//! 获取当前跟踪堆栈的深度
//! \return 当前跟踪堆栈的深度
ATOM_DBGHLP_API unsigned ATOM_GetStackTraceDepth (void);

//! 获取当前跟踪堆栈中的某个条目的函数名称
//! \param index 条目的索引
//! \return 条目中的函数名称
ATOM_DBGHLP_API const char *ATOM_GetStackTraceEntry (unsigned index);

//! 获取格式化后的当前跟踪堆栈函数列表
//! \return 格式化后的当前跟踪堆栈函数列表
ATOM_DBGHLP_API const char *ATOM_GetStackTraceString (void);

#endif // __ATOM3D_DBGHLP_STACKTRACE_H

/*! @} */
