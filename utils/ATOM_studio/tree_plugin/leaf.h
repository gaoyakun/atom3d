#ifndef __ATOM3D_STUDIO_LEAF_H
#define __ATOM3D_STUDIO_LEAF_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(SUPPORT_BILLBOARD_TREE)

class TreeNode;

class LeafModel: public ATOM_Drawable
{
public:
	LeafModel (TreeNode *node);
	bool isValid (void) const;
	void setTextureFileName (const char *filename);
	const char *getTextureFileName (void) const;
	void writeXML (ATOM_TiXmlElement *root);
	bool loadXML (ATOM_TiXmlElement *root, unsigned randomSeed);
	bool regenerate (unsigned randomSeed);
	const ATOM_BBox &getBBox (void) const;
	void setDrawBillboard (bool b);
	bool getDrawBillboard (void) const;
	void setVisible (bool b);
	bool isVisible (void) const;
	bool render (ATOM_RenderDevice *device);
	bool renderToBillboard (ATOM_RenderDevice *device);
	void setWidth (float val);
	float getWidth (void) const;
	void setHeight (float val);
	float getHeight (void) const;
	void setRandomRange (float val);
	float getRandomRange (void) const;
	void setSizeV (float val);
	float getSizeV (void) const;
	void setDensity (int val);
	int getDensity (void) const;
	void setFlip (bool val);
	bool getFlip (void) const;
	unsigned getVertexCount (void) const;
	unsigned getFaceCount (void) const;
	ATOM_Material *getMaterial (void) const;
	bool exportToFile (ATOM_File *file) const;

public:
	bool draw (ATOM_RenderDevice *device, ATOM_Camera *camera, ATOM_Material *material);

private:
	void updateLeafCards (ATOM_Node *treeNode);
	void addLeafCards (const ATOM_Vector3f *centerPoints, unsigned count, bool flip, float widthV, float heightV);

private:
	TreeNode *treeNode;
	float width;
	float height;
	float randomRange;
	float sizeV;
	int density;
	bool flip;
	unsigned seed;
	bool drawBillboard;

	struct LeafCard
	{
		ATOM_Vector3f centerPoint;
		float width;
		float height;
		bool flip;
	};
	ATOM_VECTOR<LeafCard> leaves;

	ATOM_AUTOREF(ATOM_VertexArray) vertexArray;
	ATOM_AUTOREF(ATOM_IndexArray) indexArray;
	ATOM_AUTOPTR(ATOM_Material) material;
	ATOM_AUTOREF(ATOM_Texture) texture;
	ATOM_VertexDecl vertexDecl;
	bool visible;
	ATOM_BBox bbox;
	ATOM_STRING textureFileName;
};

#endif

#endif // __ATOM3D_STUDIO_LEAF_H
