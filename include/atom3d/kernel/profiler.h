/**	\file profiler.h
 *	���ܲ�����صĹ�����ͺ�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_PROFILER_H
#define __ATOM_KERNEL_PROFILER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

//! ��ʼ��һ������������м�ʱ
//! \param id �����������
extern"C" ATOM_KERNEL_API void ATOM_CALL ATOM_BeginProfile (const char *id);

//! ��ɶ�һ������������м�ʱ
//! \param id �����������
extern"C" ATOM_KERNEL_API void ATOM_CALL ATOM_EndProfile (const char *id);

//! ��ӡ�����н��м�ʱ������ļ�ʱ���
extern"C" ATOM_KERNEL_API void ATOM_CALL ATOM_ProfileReport (void);

struct ATOM_Profiler
{
  const char *str;

  ATOM_Profiler (const char *id): str(id) {
    ATOM_BeginProfile (id);
  }

  ~ATOM_Profiler () {
    ATOM_EndProfile (str);
  }
};

#if defined(ENABLE_KERNEL_PROFILER)
# define ATOM_PROFILE(id) ::ATOM_Profiler __profiler(#id);
#else
# define ATOM_PROFILE(id)
#endif // defined(ENABLE_KERNEL_PROFILER)

#endif // __ATOM_KERNEL_PROFILER_H
/*! @} */
