/**	\file operation.h
 *	AS_Operation�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_OPERATION_H
#define __ATOM3D_STUDIO_OPERATION_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class AS_Operation
//! �ɳ����������Ķ�������
//! \author ������
//! \ingroup studio
class AS_Operation
{
public:
	virtual ~AS_Operation (void) {}
	virtual void undo (void) = 0;
	virtual void redo (void) = 0;
	virtual void deleteMe (void) = 0;
};

#endif // __ATOM3D_STUDIO_OPERATION_H

/*! @} */
