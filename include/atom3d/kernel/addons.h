/**	\file addons.h
 *	һЩ��������������.
 *
 *	\author ������
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
//! ��׼����ص�.
//! ������ATOM_AddStandardOutputCallbackע���˱�׼����ص�������ʱ�򣬸���ػ��׼���������OutputString�ӿں���
//! \author ������
class ATOM_StdOutputCallback
{
public:
  virtual ~ATOM_StdOutputCallback (void) {}
  virtual void OutputString (const char *str) = 0;
};

extern"C" {

//! ��ӱ�׼����ص�
//! \param stdoutCallback ��׼����ص�
//! \param stderrCallback ��׼��������ص�
ATOM_KERNEL_API void ATOM_CALL ATOM_AddStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback);

//! ɾ����׼����ص�
//! \param stdoutCallback ��׼����ص�
//! \param stderrCallback ��׼��������ص�
ATOM_KERNEL_API void ATOM_CALL ATOM_RemoveStandardOutputCallback (ATOM_StdOutputCallback *stdoutCallback, ATOM_StdOutputCallback *stderrCallback);

//! ˢ�����б�׼���
//! \note ������ô˺�����ʹ�ûص�����������
ATOM_KERNEL_API void ATOM_CALL ATOM_FlushStdOutputs (void);

} // extern"C"

#endif // __ATOM_KERNEL_ADDONS_H
/*! @} */
