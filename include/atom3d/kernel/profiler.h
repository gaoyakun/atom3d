/**	\file profiler.h
 *	性能测试相关的工具类和函数声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_PROFILER_H
#define __ATOM_KERNEL_PROFILER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

//! 开始对一个语句块进行运行计时
//! \param id 该语句块的描述
extern"C" ATOM_KERNEL_API void ATOM_CALL ATOM_BeginProfile (const char *id);

//! 完成对一个语句块进行运行计时
//! \param id 该语句块的描述
extern"C" ATOM_KERNEL_API void ATOM_CALL ATOM_EndProfile (const char *id);

//! 打印出所有进行计时的语句块的计时结果
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
