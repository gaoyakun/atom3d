#ifndef __ATOM3D_STUDIO_TRUNK_H
#define __ATOM3D_STUDIO_TRUNK_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(SUPPORT_BILLBOARD_TREE)

#include "treedata.h"

class TreeNode;

class TrunkModel: public ATOM_Drawable
{
private:
	ATOM_AUTOREF(ATOM_VertexArray) vertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) indexArray;
	ATOM_AUTOREF(ATOM_VertexArray) alternateVertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) alternateIndexArray;
	ATOM_AUTOPTR(ATOM_Material) material;
	ATOM_AUTOREF(ATOM_Texture) texture;
	ATOM_STRING textureFileName;
	ATOM_STRING modelFileName;
	ATOM_VertexDecl vertexDecl;
	ATOM_BBox bboxNative;
	ATOM_BBox bboxAlternate;
	bool visible;
	bool drawAlternate;
	bool drawBillboard;
	TreeData treeData;
	TreeNode *treeNode;

public:
	TrunkModel (TreeNode *node);
	bool isValid (void) const ;
	void setTextureFileName (const char *filename);
	const char *getTextureFileName (void) const;
	bool loadXML (ATOM_TiXmlElement *root, unsigned randomSeed);
	void writeXML (ATOM_TiXmlElement *root);
	bool regenerate (unsigned randomSeed);
	const ATOM_BBox &getBBox (void) const;
	void setDrawBillboard (bool b);
	bool getDrawBillboard (void) const;
	void setVisible (bool b);
	bool isVisible (void) const;
	const TreeData &getTreeData (void) const;
	bool setTreeData (const TreeData &other, unsigned randomSeed);
	bool render (ATOM_RenderDevice *device);
	bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);
	bool renderToBillboard (ATOM_RenderDevice *device);
	unsigned getVertexCount (void) const;
	unsigned getFaceCount (void) const;
	bool setAlternateModel (const char *filename);
	void setDrawAlternate (bool b);
	bool getDrawAlternate (void) const;
	bool exportToFile (ATOM_File *file) const;
	ATOM_Material *getMaterial (void) const;
};

#endif

#endif // __ATOM3D_STUDIO_TRUNK_H
