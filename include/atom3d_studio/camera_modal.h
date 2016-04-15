/**	\file camera_modal.h
 *	AS_CameraModal类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_CAMERA_MODAL_H
#define __ATOM3D_STUDIO_CAMERA_MODAL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_math.h"
#include "atom3d_studio.h"

//! \class AS_CameraModal
//! 编辑器内的摄像机接口
//! \author 高雅昆
//! \ingroup studio
class AS_CameraModal
{
public:
	enum
	{
		CM_BEGIN = 0,
		CM_FOV = CM_BEGIN,
		CM_NEAR,
		CM_FAR,
		CM_POSITION,
		CM_DIRECTION,
		CM_END
	};

public:
	//! 构造函数
	AS_CameraModal (void);

public:
	//! 析构函数
	virtual ~AS_CameraModal (void) {}

	//! 复位.
	//! 当渲染窗口大小发生变化的时候会自动调用此接口
	//! \param realtimeCtrl 渲染控件的指针
	virtual void reset (ATOM_RealtimeCtrl *realtimeCtrl) = 0;

	//! 处理键按下事件
	//! \param key 键值
	//! \param keymod 修饰键
	virtual bool handleKeyDown (ATOM_Key key, unsigned keymod) = 0;

	//! 处理键释放事件
	//! \param key 键值
	//! \param keymod 修饰键
	virtual bool handleKeyUp (ATOM_Key key, unsigned keymod) = 0;

	//! 处理鼠标键按下事件
	//! \param button 鼠标键
	//! \param x 鼠标在渲染控件客户区的x坐标
	//! \param y 鼠标在渲染控件客户区的y坐标
	//! \param keymod 修饰键
	virtual bool handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod) = 0;

	//! 处理鼠标键释放事件
	//! \param button 鼠标键
	//! \param x 鼠标在渲染控件客户区的x坐标
	//! \param y 鼠标在渲染控件客户区的y坐标
	//! \param keymod 修饰键
	virtual bool handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod) = 0;

	//! 处理鼠标移动事件
	//! \param x 鼠标在渲染控件客户区的x坐标
	//! \param y 鼠标在渲染控件客户区的y坐标
	virtual bool handleMouseMove (int x, int y) = 0;

	//! 处理鼠标滚轮事件
	//! \param x 鼠标在渲染控件客户区的x坐标
	//! \param y 鼠标在渲染控件客户区的y坐标
	//! \param delta 滚动值
	//! \param keymod 修饰键
	virtual bool handleMouseWheel (int x, int y, int delta, unsigned keymod) = 0;

	//! 设置缩放速度
	virtual void setZoomSpeed (float val) = 0;

	//! 获取缩放速度
	virtual float getZoomSpeed (void) const = 0;

	//! 设置移动速度
	virtual void setMoveSpeed (float val) = 0;

	//! 获取移动速度
	virtual float getMoveSpeed (void) const = 0;

	//! 设置旋转速度
	virtual void setRotateSpeed (float val) = 0;

	//! 获取旋转速度
	virtual float getRotateSpeed (void) = 0;

	//! 设置旋转半径
	virtual void setRotateRadius (float val) = 0;

	//! 获取旋转半径
	virtual float getRotateRadius (void) const = 0;

public:
	//! 设定摄像机位置
	//! \param pos 摄像机在世界空间的坐标
	virtual void setPosition (const ATOM_Vector3f &pos);

	//! 得到摄像机的位置
	//! \return 摄像机在世界空间的坐标
	virtual const ATOM_Vector3f &getPosition (void) const;

	//! 设定摄像机的视线方向
	//! \param dir 世界坐标系中的视线方向向量，无需单位化
	virtual void setDirection (const ATOM_Vector3f &dir);

	//! 获取摄像机的视线方向
	//! \return 世界坐标系中的视线方向向量，不一定是单位向量
	virtual const ATOM_Vector3f &getDirection (void) const;

	//! 设置摄像机的UP向量
	//! \param up 世界坐标系中摄像机的up向量，必须单位化!
	virtual void setUpVector (const ATOM_Vector3f &up);

	//! 获取摄像机的UP向量
	//! \return 世界坐标系中摄像机的up向量
	virtual const ATOM_Vector3f &getUpVector (void) const;

	//! 获取摄像机变换矩阵
	//! 该矩阵为D3D中视矩阵的逆
	//! \return 摄像机变换矩阵
	virtual const ATOM_Matrix4x4f &getViewMatrix (void) const;

	//! 设置宽高比
	//! \param aspect 宽高比
	virtual void setAspect (float aspect);

	//! 获取宽高比
	//! \return 宽高比
	virtual float getAspect (void) const;

	//! 设置Y方向视角大小
	//! \param rad 视角大小，单位为弧度
	virtual void setFovY (float rad);

	//! 获取Y方向视角大小
	//! \return 视角大小，单位为弧度
	virtual float getFovY (void) const;

	//! 设置近截面距离
	//! \param np 近截面距离
	virtual void setNearPlane (float np);

	//! 获取近截面距离
	//! \return 近截面距离
	virtual float getNearPlane (void) const;

	//! 设置远截面距离
	//! \param fp 远截面距离
	virtual void setFarPlane (float fp);

	//! 获取远截面距离
	//! \return 远截面距离
	virtual float getFarPlane (void) const;

	//! 获取投影矩阵
	//! \return 投影矩阵
	virtual const ATOM_Matrix4x4f &getProjectionMatrix (void) const;

	//! 设置摄像机参数编辑面板
	//! \param bar 面板指针
	virtual void setupOptions (ATOMX_TweakBar *bar);

	//! 设置摄像机参数编辑面板中的透视参数
	//! \param bar 面板指针
	virtual void setupPerspectiveOptions (ATOMX_TweakBar *bar);

	//! 设置摄像机参数编辑面板中的摄像机变换参数
	//! \param bar 面板指针
	virtual void setupTransformOptions (ATOMX_TweakBar *bar);

	//! 摄像机参数修改回调
	virtual void onParameterChanged (ATOMX_TWValueChangedEvent *event);

	//! 更新摄像机参数设置面板
	virtual void updateOptions (ATOMX_TweakBar *bar);

	//! 更新透视参数
	virtual void updatePerspectiveOptions (ATOMX_TweakBar *bar);

	//! 更新变换参数
	virtual void updateTransformOptions (ATOMX_TweakBar *bar);

private:
	ATOM_Vector3f _position;
	ATOM_Vector3f _direction;
	ATOM_Vector3f _up;
	float _aspect;
	float _nearPlane;
	float _farPlane;
	float _fovY;
	mutable ATOM_Matrix4x4f _matrix;
	mutable ATOM_Matrix4x4f _projmatrix;
	mutable bool _matrixDirty;
	mutable bool _projmatrixDirty;
};

#endif // __ATOM3D_STUDIO_CAMERA_MODAL_H

/*! @} */
