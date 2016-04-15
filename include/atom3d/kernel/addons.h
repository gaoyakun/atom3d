/**	\file addons.h
 *	一些辅助函数的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_ADDONS_H
#define __ATOM_KERNEL_ADDONS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <stddef.h>
#include <malloc.h>
#include "basedefs.h"

//! \class ATOM_StdOutputCallback
//! 标准输出回调.
//! 当调用ATOM_AddStandardOutputCallback注册了标准输出回调函数的时候，该类截获标准输出并调用OutputString接口函数
//! \author 高雅昆
class ATOM_StdOutputCallback
{
public:
  virtual ~ATOM_StdOutputCallback (void) {}
  virtual void OutputString (const char *str) = 0;
};

extern"C" {

//! 添加标准输出回调
//! \param stdoutCallback 标准输出回调
//! \param stderrCallback 标准错误输出回调
ATOM_KERNEL_API void ATOM_CALL ATOM_AddStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback);

//! 删除标准输出回调
//! \param stdoutCallback 标准输出回调
//! \param stderrCallback 标准错误输出回调
ATOM_KERNEL_API void ATOM_CALL ATOM_RemoveStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback);

//! 刷新所有标准输出
//! \note 必须调用此函数以使得回调函数起作用
ATOM_KERNEL_API void ATOM_CALL ATOM_FlushStdOutputs (void);

} // extern"C"

#endif // __ATOM_KERNEL_ADDONS_H
/*! @} */
