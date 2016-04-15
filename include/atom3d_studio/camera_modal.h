/**	\file camera_modal.h
 *	AS_CameraModal�������.
 *
 *	\author ������
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
//! �༭���ڵ�������ӿ�
//! \author ������
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
	//! ���캯��
	AS_CameraModal (void);

public:
	//! ��������
	virtual ~AS_CameraModal (void) {}

	//! ��λ.
	//! ����Ⱦ���ڴ�С�����仯��ʱ����Զ����ô˽ӿ�
	//! \param realtimeCtrl ��Ⱦ�ؼ���ָ��
	virtual void reset (ATOM_RealtimeCtrl *realtimeCtrl) = 0;

	//! ����������¼�
	//! \param key ��ֵ
	//! \param keymod ���μ�
	virtual bool handleKeyDown (ATOM_Key key, unsigned keymod) = 0;

	//! ������ͷ��¼�
	//! \param key ��ֵ
	//! \param keymod ���μ�
	virtual bool handleKeyUp (ATOM_Key key, unsigned keymod) = 0;

	//! �������������¼�
	//! \param button ����
	//! \param x �������Ⱦ�ؼ��ͻ�����x����
	//! \param y �������Ⱦ�ؼ��ͻ�����y����
	//! \param keymod ���μ�
	virtual bool handleMouseButtonDown (ATOM_MouseButton button, int x, int y, unsigned keymod) = 0;

	//! ���������ͷ��¼�
	//! \param button ����
	//! \param x �������Ⱦ�ؼ��ͻ�����x����
	//! \param y �������Ⱦ�ؼ��ͻ�����y����
	//! \param keymod ���μ�
	virtual bool handleMouseButtonUp (ATOM_MouseButton button, int x, int y, unsigned keymod) = 0;

	//! ��������ƶ��¼�
	//! \param x �������Ⱦ�ؼ��ͻ�����x����
	//! \param y �������Ⱦ�ؼ��ͻ�����y����
	virtual bool handleMouseMove (int x, int y) = 0;

	//! �����������¼�
	//! \param x �������Ⱦ�ؼ��ͻ�����x����
	//! \param y �������Ⱦ�ؼ��ͻ�����y����
	//! \param delta ����ֵ
	//! \param keymod ���μ�
	virtual bool handleMouseWheel (int x, int y, int delta, unsigned keymod) = 0;

	//! ���������ٶ�
	virtual void setZoomSpeed (float val) = 0;

	//! ��ȡ�����ٶ�
	virtual float getZoomSpeed (void) const = 0;

	//! �����ƶ��ٶ�
	virtual void setMoveSpeed (float val) = 0;

	//! ��ȡ�ƶ��ٶ�
	virtual float getMoveSpeed (void) const = 0;

	//! ������ת�ٶ�
	virtual void setRotateSpeed (float val) = 0;

	//! ��ȡ��ת�ٶ�
	virtual float getRotateSpeed (void) = 0;

	//! ������ת�뾶
	virtual void setRotateRadius (float val) = 0;

	//! ��ȡ��ת�뾶
	virtual float getRotateRadius (void) const = 0;

public:
	//! �趨�����λ��
	//! \param pos �����������ռ������
	virtual void setPosition (const ATOM_Vector3f &pos);

	//! �õ��������λ��
	//! \return �����������ռ������
	virtual const ATOM_Vector3f &getPosition (void) const;

	//! �趨����������߷���
	//! \param dir ��������ϵ�е����߷������������赥λ��
	virtual void setDirection (const ATOM_Vector3f &dir);

	//! ��ȡ����������߷���
	//! \return ��������ϵ�е����߷�����������һ���ǵ�λ����
	virtual const ATOM_Vector3f &getDirection (void) const;

	//! �����������UP����
	//! \param up ��������ϵ���������up���������뵥λ��!
	virtual void setUpVector (const ATOM_Vector3f &up);

	//! ��ȡ�������UP����
	//! \return ��������ϵ���������up����
	virtual const ATOM_Vector3f &getUpVector (void) const;

	//! ��ȡ������任����
	//! �þ���ΪD3D���Ӿ������
	//! \return ������任����
	virtual const ATOM_Matrix4x4f &getViewMatrix (void) const;

	//! ���ÿ�߱�
	//! \param aspect ��߱�
	virtual void setAspect (float aspect);

	//! ��ȡ��߱�
	//! \return ��߱�
	virtual float getAspect (void) const;

	//! ����Y�����ӽǴ�С
	//! \param rad �ӽǴ�С����λΪ����
	virtual void setFovY (float rad);

	//! ��ȡY�����ӽǴ�С
	//! \return �ӽǴ�С����λΪ����
	virtual float getFovY (void) const;

	//! ���ý��������
	//! \param np ���������
	virtual void setNearPlane (float np);

	//! ��ȡ���������
	//! \return ���������
	virtual float getNearPlane (void) const;

	//! ����Զ�������
	//! \param fp Զ�������
	virtual void setFarPlane (float fp);

	//! ��ȡԶ�������
	//! \return Զ�������
	virtual float getFarPlane (void) const;

	//! ��ȡͶӰ����
	//! \return ͶӰ����
	virtual const ATOM_Matrix4x4f &getProjectionMatrix (void) const;

	//! ��������������༭���
	//! \param bar ���ָ��
	virtual void setupOptions (ATOMX_TweakBar *bar);

	//! ��������������༭����е�͸�Ӳ���
	//! \param bar ���ָ��
	virtual void setupPerspectiveOptions (ATOMX_TweakBar *bar);

	//! ��������������༭����е�������任����
	//! \param bar ���ָ��
	virtual void setupTransformOptions (ATOMX_TweakBar *bar);

	//! ����������޸Ļص�
	virtual void onParameterChanged (ATOMX_TWValueChangedEvent *event);

	//! ��������������������
	virtual void updateOptions (ATOMX_TweakBar *bar);

	//! ����͸�Ӳ���
	virtual void updatePerspectiveOptions (ATOMX_TweakBar *bar);

	//! ���±任����
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
