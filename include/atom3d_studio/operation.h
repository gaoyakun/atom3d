/**	\file operation.h
 *	AS_Operation类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_OPERATION_H
#define __ATOM3D_STUDIO_OPERATION_H

#if _MSC_VER > 1000
# pragma once
#endif

//! \class AS_Operation
//! 可撤销或重做的动作对象
//! \author 高雅昆
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
