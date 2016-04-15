/**	\file guihud.h
 *	2D平面矩形图像节点类.
 *
 *	\author 高雅昆
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
//! 2D平面矩形图像节点.
//! 和ATOM_Billboard类不同，此节点不会有近大远小的透视效果
//! \note 调用此节点类的包围盒函数无效
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_Hud: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_Hud, Hud)

public:
	//! 对齐方式
	enum Alignment
	{
		ALIGN_CENTER = 0,
		ALIGN_LEFT = (1<<0),
		ALIGN_RIGHT = (1<<1),
		ALIGN_TOP = (1<<2),
		ALIGN_BOTTOM = (1<<3)
	};

	//! 文字属性
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

	//! 回调函数用来在渲染前修改屏幕坐标
	typedef ATOM_Vector3f (ATOM_CALL *PositionCallback) (const ATOM_Vector3f &screenPosition, void *userdata);

public:
	//! 构造函数.
	ATOM_Hud (void);

	//! 析构函数.
	virtual ~ATOM_Hud (void);

public:
	//! 设置平面矩形的中心点位置的视口坐标.
	//! 此参数在使用屏幕坐标系时才有用
	//! \sa setScreenSpace getPosition
	//! \param x x坐标,从左到右的方向
	//! \param y y坐标,从上到下的方向
	//! \sa getPosition
	void setPosition (int x, int y);

	//! 返回平面矩形中心点位置的视口坐标
	//! \param x x坐标,从左到右的方向
	//! \param y y坐标,从上到下的方向
	//! \sa setPosition
	void getPosition (int &x, int &y) const;

	//! 设置平面矩形的z值.
	//! 此参数在使用屏幕坐标系时才有用
	//! \sa setScreenSpace getScreenSpace getZ
	//! \param z z值
	void setZ (float z);

	//! 获取平面矩形的z值
	//! \return z值
	//! \sa setScreenSpace getScreenSpace setZ
	float getZ (void) const;

	//! 设置是否使用屏幕空间坐标
	//! \param b 为true表示使用屏幕空间坐标
	//! \sa isScreenSpace
	void setScreenSpace (bool b);

	//! 查询位置坐标是否为屏幕空间的坐标.
	//! \return true是屏幕空间
	//! \sa setScreenSpace setPosition getPosition
	bool isScreenSpace (void) const;

	//! 设置屏幕空间宽度
	//! \param width 宽度,单位为像素
	//! \sa getWidth
	void setWidth (unsigned width);

	//! 设置屏幕空间高度
	//! \param height 高度,单位为像素
	//! \sa getHeight
	void setHeight (unsigned height);

	//! 获取屏幕空间宽度
	//! \return 宽度,单位为像素
	//! \sa setWidth
	unsigned getWidth (void) const;

	//! 获取屏幕空间高度此
	//! \return 高度,单位为像素
	//! \sa setHeight
	unsigned getHeight (void) const;

	//! 设置矩形框颜色
	//! \param color 颜色值,Alpha通道的值控制其透明度
	//! \sa getColor
	void setColor (ATOM_ColorARGB color);

	//! 返回矩形框颜色
	//! \return 颜色值
	//! \sa setColor
	ATOM_ColorARGB getColor (void) const;

	//! 设置矩形框贴图
	//! \param texture 贴图指针,此贴图将会使用点采样
	//! \sa getTexture
	//! \todo 如果系统支持npo2贴图，可以给选项支持线性采样以及Mipmap
	void setTexture (ATOM_Texture *texture);

	//! 获取矩形框贴图
	//! \return 贴图指针
	//! \sa setTexture
	ATOM_Texture *getTexture (void) const;

	//! 设置文字属性
	//! \param textProp 文字属性
	//! \sa getTextProperties
	void setTextProperties (const TextProp &textProp);

	//! 获取文字属性
	//! \param textProp 文字属性
	//! \sa setTextProperties
	//! \note 返回的textProp结构中的字符串指针不能保证长期有效
	void getTextProperties (TextProp &textProp) const;

	//! 获取材质
	//! \return 材质指针
	ATOM_Material *getMaterial (void);

	//! 获取屏幕空间中心点位置和深度
	//! \return 中心点位置深度
	const ATOM_Vector3f &getCenterPosition (void) const;

	//! 设置对齐方式
	//! \param alignment 对齐方式，为Aligment枚举量的组合
	//! \sa Alignment getAlignment
	void setAlignment (unsigned alignment);

	//! 获取对齐方式
	//! \return 对齐方式，为Alignment枚举量的组合
	//! \sa Alignment setAlignment
	unsigned getAlignment (void) const;

	//! 设置回调
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
