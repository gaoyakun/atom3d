/**	\file send_error.h
 *	��ջ����֧��
 *
 *	\author ������
 *	\addtogroup dbghlp
 *	@{
 */

#ifndef __ATOM3D_DBGHLP_STACKTRACE_H
#define __ATOM3D_DBGHLP_STACKTRACE_H

#include "basedefs.h"

#define ATOM_NO_STACK_TRACE

/**
	\def ATOM_STACK_TRACE(func)
	�ں������һ��ʹ�ô˺��ʹ����\a func֧�ֶ�ջ����
	\code
		void Foo (void)
		{
			ATOM_STACK_TRACE(Foo)
			// do some thing
		}
	\endcode
	������ջ���ٶ�������һ��Ӱ�죬����ֻ�Բ��ֺ���ʵ�ֶ�ջ����
	Ҫ���ö�ջ���٣�����Ŀ�ж���ATOM_NO_STACK_TRACE
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

//! ��ȡ��ǰ���ٶ�ջ�����
//! \return ��ǰ���ٶ�ջ�����
ATOM_DBGHLP_API unsigned ATOM_GetStackTraceDepth (void);

//! ��ȡ��ǰ���ٶ�ջ�е�ĳ����Ŀ�ĺ�������
//! \param index ��Ŀ������
//! \return ��Ŀ�еĺ�������
ATOM_DBGHLP_API const char *ATOM_GetStackTraceEntry (unsigned index);

//! ��ȡ��ʽ����ĵ�ǰ���ٶ�ջ�����б�
//! \return ��ʽ����ĵ�ǰ���ٶ�ջ�����б�
ATOM_DBGHLP_API const char *ATOM_GetStackTraceString (void);

#endif // __ATOM3D_DBGHLP_STACKTRACE_H

/*! @} */
