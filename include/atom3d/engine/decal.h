#ifndef __ATOM3D_ENGINE_DECAL_H
#define __ATOM3D_ENGINE_DECAL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "node.h"

class ATOM_ENGINE_API ATOM_Decal: public ATOM_Node, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_Decal, ATOM_Decal)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Decal)

public:
	ATOM_Decal (void);
	virtual ~ATOM_Decal (void);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool supportMTLoading (void);
	virtual bool supportFixedFunction (void) const;
	virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

	//--- wangjian added ---//
	// 设置绘制排序标记
	virtual void setupSortFlag(int queueid);
protected:
	virtual void resetMaterialDirtyFlag_impl();
	//----------------------//

protected:
	virtual bool onLoad (ATOM_RenderDevice *device);
	virtual void buildBoundingbox (void) const;
	virtual void onTransformChanged (void);
	virtual void assign (ATOM_Node *other) const;

public:
	void setTexture (ATOM_Texture *texture);
	ATOM_Texture *getTexture (void) const;

public:	
	void setTextureFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getTextureFileName (void) const;
	void setSrcBlend(int srcBlend);
	int getSrcBlend (void) const;
	void setDestBlend (int destBlend);
	int getDestBlend (void) const;
	void enableBlend (int enable);
	int isBlendEnabled (void) const;
	void setColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getColor (void) const;
	void setMaterial (ATOM_Material *material);
	ATOM_Material *getMaterial (void) const;

private:
	ATOM_STRING _textureFileName;
	ATOM_AUTOREF(ATOM_Texture) _texture;
	ATOM_AUTOPTR(ATOM_Material) _material;
	int _enableBlend;
	int _srcBlend;
	int _destBlend;
	ATOM_Vector4f _color;

	//--- wangjian added ---//
	// 异步加载：检查异步加载完成标记
	void AddStreamingTask();
	//----------------------//
};

#endif // __ATOM3D_ENIGNE_DECAL_H
