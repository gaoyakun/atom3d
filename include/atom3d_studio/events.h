/**	\file camera_modal.h
 *	Studio�ڲ��¼�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_EVENTS_H
#define __ATOM3D_STUDIO_EVENTS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_kernel.h"

//! \class AS_AssetCreateEvent
//! ������Դ�¼�.
//! ���û���Asset����е��������Դ������¼��ᱻ�ַ�����չ���
//! \author ������
//! \ingroup studio
class AS_AssetCreateEvent: public ATOM_Event
{
public:
	AS_AssetCreateEvent (void);
	AS_AssetCreateEvent (const char *name_, const char *filename_, const char *desc_, float rotateX_, float rotateY_, float rotateZ_, float offsetY_);

	const char *name;
	const char *filename;
	const char *desc;
	float randomRotateX;
	float randomRotateY;
	float randomRotateZ;
	float offsetY;

	ATOM_DECLARE_EVENT(AS_AssetCreateEvent)
};

class AS_AssetReplaceEvent: public ATOM_Event
{
public:
	AS_AssetReplaceEvent (void);
	AS_AssetReplaceEvent (const char *name_, const char *filename_, const char *desc_);

	const char *name;
	const char *filename;
	const char *desc;

	ATOM_DECLARE_EVENT(AS_AssetReplaceEvent)
};

#endif // __ATOM3D_STUDIO_EVENTS_H

/*! @} */
