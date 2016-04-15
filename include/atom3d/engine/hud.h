/**	\file guihud.h
 *	2Dƽ�����ͼ��ڵ���.
 *
 *	\author ������
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_HUD_H
#define __ATOM3D_ENGINE_HUD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_utils.h"
#include "../ATOM_ttfont.h"
#include "node.h"

//! \class ATOM_Hud
//! 2Dƽ�����ͼ��ڵ�.
//! ��ATOM_Billboard�಻ͬ���˽ڵ㲻���н���ԶС��͸��Ч��
//! \note ���ô˽ڵ���İ�Χ�к�����Ч
//! \author ������
//! \ingroup engine
class ATOM_ENGINE_API ATOM_Hud: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_Hud, Hud)

public:
	//! ���뷽ʽ
	enum Alignment
	{
		ALIGN_CENTER = 0,
		ALIGN_LEFT = (1<<0),
		ALIGN_RIGHT = (1<<1),
		ALIGN_TOP = (1<<2),
		ALIGN_BOTTOM = (1<<3)
	};

	//! ��������
	struct TextProp
	{
		char *text;
		ATOM_ColorARGB textColor;
		ATOM_FontHandle fontHandle;
		int charMargin;
		int x;
		int y;
		int borderX;
		int borderY;
		unsigned alignment;
	};

	//! �ص�������������Ⱦǰ�޸���Ļ����
	typedef ATOM_Vector3f (ATOM_CALL *PositionCallback) (const ATOM_Vector3f &screenPosition, void *userdata);

public:
	//! ���캯��.
	ATOM_Hud (void);

	//! ��������.
	virtual ~ATOM_Hud (void);

public:
	//! ����ƽ����ε����ĵ�λ�õ��ӿ�����.
	//! �˲�����ʹ����Ļ����ϵʱ������
	//! \sa setScreenSpace getPosition
	//! \param x x����,�����ҵķ���
	//! \param y y����,���ϵ��µķ���
	//! \sa getPosition
	void setPosition (int x, int y);

	//! ����ƽ��������ĵ�λ�õ��ӿ�����
	//! \param x x����,�����ҵķ���
	//! \param y y����,���ϵ��µķ���
	//! \sa setPosition
	void getPosition (int &x, int &y) const;

	//! ����ƽ����ε�zֵ.
	//! �˲�����ʹ����Ļ����ϵʱ������
	//! \sa setScreenSpace getScreenSpace getZ
	//! \param z zֵ
	void setZ (float z);

	//! ��ȡƽ����ε�zֵ
	//! \return zֵ
	//! \sa setScreenSpace getScreenSpace setZ
	float getZ (void) const;

	//! �����Ƿ�ʹ����Ļ�ռ�����
	//! \param b Ϊtrue��ʾʹ����Ļ�ռ�����
	//! \sa isScreenSpace
	void setScreenSpace (bool b);

	//! ��ѯλ�������Ƿ�Ϊ��Ļ�ռ������.
	//! \return true����Ļ�ռ�
	//! \sa setScreenSpace setPosition getPosition
	bool isScreenSpace (void) const;

	//! ������Ļ�ռ���
	//! \param width ���,��λΪ����
	//! \sa getWidth
	void setWidth (unsigned width);

	//! ������Ļ�ռ�߶�
	//! \param height �߶�,��λΪ����
	//! \sa getHeight
	void setHeight (unsigned height);

	//! ��ȡ��Ļ�ռ���
	//! \return ���,��λΪ����
	//! \sa setWidth
	unsigned getWidth (void) const;

	//! ��ȡ��Ļ�ռ�߶ȴ�
	//! \return �߶�,��λΪ����
	//! \sa setHeight
	unsigned getHeight (void) const;

	//! ���þ��ο���ɫ
	//! \param color ��ɫֵ,Alphaͨ����ֵ������͸����
	//! \sa getColor
	void setColor (ATOM_ColorARGB color);

	//! ���ؾ��ο���ɫ
	//! \return ��ɫֵ
	//! \sa setColor
	ATOM_ColorARGB getColor (void) const;

	//! ���þ��ο���ͼ
	//! \param texture ��ͼָ��,����ͼ����ʹ�õ����
	//! \sa getTexture
	//! \todo ���ϵͳ֧��npo2��ͼ�����Ը�ѡ��֧�����Բ����Լ�Mipmap
	void setTexture (ATOM_Texture *texture);

	//! ��ȡ���ο���ͼ
	//! \return ��ͼָ��
	//! \sa setTexture
	ATOM_Texture *getTexture (void) const;

	//! ������������
	//! \param textProp ��������
	//! \sa getTextProperties
	void setTextProperties (const TextProp &textProp);

	//! ��ȡ��������
	//! \param textProp ��������
	//! \sa setTextProperties
	//! \note ���ص�textProp�ṹ�е��ַ���ָ�벻�ܱ�֤������Ч
	void getTextProperties (TextProp &textProp) const;

	//! ��ȡ����
	//! \return ����ָ��
	ATOM_Material *getMaterial (void);

	//! ��ȡ��Ļ�ռ����ĵ�λ�ú����
	//! \return ���ĵ�λ�����
	const ATOM_Vector3f &getCenterPosition (void) const;

	//! ���ö��뷽ʽ
	//! \param alignment ���뷽ʽ��ΪAligmentö���������
	//! \sa Alignment getAlignment
	void setAlignment (unsigned alignment);

	//! ��ȡ���뷽ʽ
	//! \return ���뷽ʽ��ΪAlignmentö���������
	//! \sa Alignment setAlignment
	unsigned getAlignment (void) const;

	//! ���ûص�
	void setPositionCallback (PositionCallback calback, void *userdata);

public:
	//! \copydoc ATOM_Node::accept
	virtual void accept (ATOM_Visitor &visitor);

protected:
	//! \copydoc ATOM_Node::onLoad
	virtual bool onLoad (ATOM_RenderDevice *device);

public:
	//! \copydoc ATOM_BaseRenderDatas::render
    virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

	//! \copydoc ATOM_BaseRenderDatas::computeDistanceSq
	virtual float computeSquaredDistanceToCamera (ATOM_Camera *camera, const ATOM_Matrix4x4f &worldMatrix);

protected:
    virtual void buildBoundingbox() const;

public:
	virtual void update (const ATOM_Rect2Di &viewport, const ATOM_Matrix4x4f &mvp);

protected:
	bool _screenSpace;
	int _x;
	int _y;
	float _z;
	unsigned _width;
	unsigned _height;
	unsigned _alignment;
	ATOM_ColorARGB _diffuseColor;
	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_AUTOREF(ATOM_Text) _text;
	TextProp _textProp;
	int _textRect[4];
	ATOM_Vector3f _screenCenter;
	PositionCallback _callback;
	void *_userdata;

	ATOM_AUTOPTR(ATOM_Material) _material;
};

#endif // __ATOM3D_ENGINE_HUD_H
/*! @} */
