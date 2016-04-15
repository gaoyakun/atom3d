/**	\file colorgradingeffect.h
 *	ATOM_ColorGradingEffect�������.
 *
 *	\author ������
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_COLORGRADINGEFFECT_H
#define __ATOM3D_ENGINE_COLORGRADINGEFFECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "posteffectchain.h"

//! \class ATOM_ColorGradingEffect
//! Уɫ����Ч����
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ColorGradingEffect: public ATOM_PostEffect
{
	ATOM_CLASS(engine, ATOM_ColorGradingEffect, ATOM_ColorGradingEffect)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ColorGradingEffect)

public:
	//! ���캯��
	ATOM_ColorGradingEffect (void);

	//! ���캯��
	//! \param lutTexture ��ɫ�����ͼ���ɱ༭������
	//! \param linearFilter trueʹ��˫���Թ��� falseʹ�õ����
	ATOM_ColorGradingEffect (ATOM_Texture *lutTexture);

	//! ��������
	virtual ~ATOM_ColorGradingEffect (void);

public:
	//! ������ɫ�����ͼ
	//! \param lutTexture ��ɫ�����ͼ���ɱ༭������
	void setLUT (ATOM_Texture *lutTexture);

	//! ��ȡ��ɫ�����ͼ
	//! \return ��ɫ�����ͼ
	ATOM_Texture *getLUT (void) const;

	//! ������ɫ�����ͼ
	//! \param lutTexture ��ɫ�����ͼ���ɱ༭������
	void setLUTTextureFileName (const ATOM_STRING &fileName);

	//! ��ȡ��ɫ�����ͼ
	//! \return ��ɫ�����ͼ
	const ATOM_STRING &getLUTTextureFileName (void) const;

private:
	void createDefaultLutTexture (ATOM_RenderDevice *device);

public:
	virtual bool render (ATOM_RenderDevice *device);

private:
	ATOM_AUTOREF(ATOM_Texture) _lutTexture;
	ATOM_AUTOPTR(ATOM_Material) _material;
	ATOM_STRING _lutTextureFileName;
	bool _textureDirty;
	bool _materialError;
};


#endif // __ATOM3D_ENGINE_COLORGRADINGEFFECT_H

/*! @} */
