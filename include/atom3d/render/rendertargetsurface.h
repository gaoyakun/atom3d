//////////////////////////////////////////////
// wangjian added . 
// 时间 ：	2013/6/13
// 描述 ：	新增一个RT surface的资源类型。
//			用于某些特殊情况下(比如：shadow mapping时需要的NULL RENDER TARGET支持)
//			直接设置RENDER TARGET的SURFACE.
//////////////////////////////////////////////

#ifndef __ATOM3D_RENDER_RTSURFACE_H
#define __ATOM3D_RENDER_RTSURFACE_H

#include "basedefs.h"
#include "gfxtypes.h"
#include "gfxresource.h"


class ATOM_RenderTargetSurface : public ATOM_GfxResource
{
public:
	enum ATOM_RTSURFACE_FORMAT
	{
		ATOM_RTSURFACE_FMT_NULLRT,
		ATOM_RTSURFACE_FMT_ALL,
	};
	
protected:
    ATOM_RenderTargetSurface(void);
	virtual ~ATOM_RenderTargetSurface (void);

public:
    virtual void setRenderDevice (ATOM_RenderDevice *device);
    virtual unsigned getWidth (void) const
	{
		return _width;
	}
    virtual unsigned getHeight (void) const
	{
		return _height;
	}
	virtual ATOM_RTSURFACE_FORMAT getFormat(void) const
	{
		return _format;
	}

	

protected:
	virtual bool create (unsigned width, unsigned height,ATOM_RTSURFACE_FORMAT fmt) = 0;

protected:
	unsigned				_width;
	unsigned				_height;
	ATOM_RTSURFACE_FORMAT	_format;
};

#endif // __ATOM3D_RENDER_RTSURFACE_H
