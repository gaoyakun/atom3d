/**	\file shape.h
 *	ShapeNode类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_SHAPENODE_H
#define __ATOM3D_ENGINE_SHAPENODE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "visualnode.h"

//--- wangjian added ---//
#include "ayncLoader.h"
//----------------------//

//! \class ATOM_ShapeNode
//! 简单形状节点
//! \author 高雅昆
//! \ingroup engine
class ATOM_ENGINE_API ATOM_ShapeNode: public ATOM_VisualNode, public ATOM_Drawable
{
	ATOM_CLASS(engine, ATOM_ShapeNode, ATOM_ShapeNode)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_ShapeNode)

public:
	enum
	{
		PLANE = 0,
		SPHERE = 1,
		CYLINDER = 2,
		CONE = 3,
		BILLBOARD = 4,
		TEXT = 5,
		GRID = 6,
		MODEL = 7
	};

public:
	ATOM_ShapeNode (void);
	virtual ~ATOM_ShapeNode (void);

public:
	virtual void buildBoundingbox (void) const;
	virtual void setupRenderQueue (ATOM_CullVisitor *visitor);
	virtual bool rayIntersectionTest (ATOM_Camera *camera, const ATOM_Ray &ray, float *len) const;
	virtual bool loadAttribute(const ATOM_TiXmlElement *xmlelement);
	virtual bool writeAttribute(ATOM_TiXmlElement *xmlelement);

public:
	virtual void accept (ATOM_Visitor &visitor);
	virtual bool draw(ATOM_RenderDevice* device, ATOM_Camera *camera, ATOM_Material *material);

	//--- wangjian added ---//
	virtual void prepareForSort(ATOM_RenderScheme* renderScheme);
	// 设置绘制排序标记
	virtual void setupSortFlag(int queueid);
#if 0
	// 对于model类型的SHAPE节点 使用实例化渲染（如果开启实例化）
	virtual bool drawBatch(	ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material, 
							bool bInstancing = true, sInstancing * instancing = 0	);
	virtual void batching(sInstancing * instanceWrapper, ATOM_Camera *camera);
#endif
	//----------------------//

public:
	void setType (int type);
	int getType (void) const;
	void setMaterialFileName (const ATOM_STRING &materialFileName);
	const ATOM_STRING &getMaterialFileName (void) const;
	void setMaterial (ATOM_Material *material);
	ATOM_Material *getMaterial (void) const;
	void setWithCaps (int b);
	int getWithCaps (void) const;
	void setSmoothness (int smoothness);
	int getSmoothness (void) const;
	void setDivisionX (int division);
	int getDivisionX (void) const;
	void setDivisionY (int division);
	int getDivisionY (void) const;
	void setVisiblePart (int part);
	int getVisiblePart (void) const;
	void setTransparency (float val);
	float getTransparency (void) const;
	void setShearing (float shearing);
	float getShearing (void) const;
	void setSize (const ATOM_Vector3f &size);
	const ATOM_Vector3f &getSize (void) const;
	void setColor (const ATOM_Vector4f &color);
	const ATOM_Vector4f &getColor (void) const;
	void setText (const char *str);
	const char *getText (void) const;
	void setUVRotation (float uvRotation);
	float getUVRotation (void) const;
	void setUVScaleOffset (const ATOM_Vector4f &v);
	const ATOM_Vector4f &getUVScaleOffset (void) const;
	void setModelFileName (const ATOM_STRING &fileName);
	const ATOM_STRING &getModelFileName (void) const;

	//---- wangjian added  ----//
	//! 设置全局颜色强度因子
	void setColorMultiplier (const float colorMultiplier);
	//! 获取全局颜色强度因子
	const float getColorMultiplier (void) const;
	//! 设置全局溶解强度因子
	void setResolveScale (const float resolveFactor);
	//! 获取全局溶解强度因子
	const float getResolveScale (void) const;
	//-------------------------//
protected:
	virtual bool onLoad(ATOM_RenderDevice *device);

private:
	bool regenerate (ATOM_RenderDevice *device);
	bool loadModel (const char *fileName);

private:
	int _type;
	int _withCaps;
	int _smoothness;
	int _divideX;
	int _divideY;
	int _visiblePart;
	float _transparency;
	float _shearing;
	bool _typeDirty;
	float _uvRotation;
	ATOM_Vector4f _uvScaleOffset;
	ATOM_Vector4f _color;
	ATOM_Vector3f _size;
	ATOM_STRING _text;
	ATOM_STRING						_materialFileName;
	ATOM_STRING						_modelFileName;
	float							_colorMultiplier;						// wangjian added
	float							_resolveFactor;						// wangjian added

	struct ShapeVertex
	{
		ATOM_Vector3f pos;
		ATOM_Vector3f normal;
		ATOM_Vector2f uv;
	};
	ATOM_VECTOR<ShapeVertex> _vertices;
	ATOM_VECTOR<unsigned short> _indices;

	ATOM_AUTOREF(ATOM_VertexArray)	_vertexArray;
	ATOM_AUTOREF(ATOM_IndexArray)	_indexArray;
	ATOM_AUTOPTR(ATOM_Material)		_material;
	ATOM_AUTOREF(ATOM_SharedModel)	_model;
	static ATOM_VertexDecl _vertexDecl;
	ATOM_AABBTree *_aabbTree;

	ATOM_ParameterTable::ValueHandle _hTransparency;
	ATOM_ParameterTable::ValueHandle _hDiffuseColor;
	ATOM_ParameterTable::ValueHandle _hUVRotation;
	ATOM_ParameterTable::ValueHandle _hUVScaleOffset;
	ATOM_ParameterTable::ValueHandle _hProjectable;
	ATOM_ParameterTable::ValueHandle _hColorMultiplier;
	ATOM_ParameterTable::ValueHandle _hResolveFactor;	// wangjian added

#if 0
	ATOM_ParameterTable::ValueHandle _hDiffuseTexture;	// 
	ATOM_ParameterTable::ValueHandle _hPerturbTexture;	//
	ATOM_ParameterTable::ValueHandle _hPerturbMaskTex;	//
	ATOM_ParameterTable::ValueHandle _hDisolveTexture;	//
#endif
	
	ATOM_DECLARE_NODE_FILE_TYPE(ATOM_ShapeNode, ATOM_ShapeNode, "shape", "ATOM3D Shape")

	//--- wangjian added ---//
public:
	// 当该节点完全加载完成时调用
	virtual void onLoadFinished();

protected:
	virtual void resetMaterialDirtyFlag_impl();
	//----------------------//
};

#endif // __ATOM3D_ENGINE_SHAPENODE_H
/*! @} */
